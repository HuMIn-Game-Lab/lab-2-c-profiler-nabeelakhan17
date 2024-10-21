import json
import pandas as pd
import plotly.express as px
from dash import Dash, dcc, html, Input, Output, dash_table
import dash_bootstrap_components as dbc
import logging
import numpy as np
import dash
from transformers import AutoTokenizer, AutoModelForSeq2SeqLM
import torch

# Configure logging
logging.basicConfig(
    level=logging.DEBUG,  # Capture all log messages
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    handlers=[
        logging.FileHandler("app.log"),
        logging.StreamHandler()
    ]
)
logger = logging.getLogger(__name__)

# Initialize Dash app with a Bootstrap theme
app = Dash(__name__, external_stylesheets=[dbc.themes.BOOTSTRAP])

def load_profiler_data(json_file):
    """Load profiler data from a JSON file."""
    try:
        with open(json_file, 'r') as f:
            data = json.load(f)
        logger.info(f"Successfully loaded data from {json_file}.")
        logger.debug(f"Profiler data loaded: {data}")
        return data
    except FileNotFoundError:
        logger.error(f"Error: The file {json_file} was not found.")
        return {}
    except json.JSONDecodeError as e:
        logger.error(f"Error: The file {json_file} is not a valid JSON. {e}")
        return {}

def preprocess_data(profiler_data):
    """Preprocess profiler data into a DataFrame."""
    if 'sections' not in profiler_data:
        logger.error("Error: JSON data must contain 'sections' key.")
        return pd.DataFrame()

    sections = pd.DataFrame(profiler_data.get('sections', []))

    # Ensure necessary columns exist, fill missing with default values
    required_columns = {'sectionName', 'count', 'totalTime', 'averageTime',
                        'minTime', 'maxTime', 'fileName', 'functionName', 'lineNumber',
                        'startTime', 'endTime', 'parentSection'}
    missing_columns = required_columns - set(sections.columns)
    if missing_columns:
        logger.warning(f"Missing columns in 'sections' data: {missing_columns}. Filling with default values.")
        for col in missing_columns:
            if col in {'count', 'totalTime', 'averageTime', 'minTime', 'maxTime',
                       'lineNumber', 'startTime', 'endTime'}:
                sections[col] = 0
            elif col == 'parentSection':
                sections[col] = None
            else:
                sections[col] = 'Unknown'

    # Convert numeric columns to appropriate data types
    numeric_columns = ['count', 'totalTime', 'averageTime', 'minTime', 'maxTime',
                       'lineNumber', 'startTime', 'endTime']
    sections[numeric_columns] = sections[numeric_columns].apply(pd.to_numeric, errors='coerce')

    # Check for NaNs in numeric columns
    logger.debug(f"Numeric columns data types:\n{sections[numeric_columns].dtypes}")
    logger.debug(f"NaN values in numeric columns:\n{sections[numeric_columns].isna().sum()}")

    # Log numeric columns after conversion
    logger.debug(f"Numeric columns after conversion:\n{sections[numeric_columns].head()}")

    # Drop rows with NaNs in critical columns
    sections.dropna(subset=['averageTime', 'minTime', 'maxTime'], inplace=True)

    # Remove rows with zero or negative averageTime
    sections = sections[sections['averageTime'] > 0]

    # Log DataFrame info
    logger.info(f"Sections DataFrame shape: {sections.shape}")
    logger.info(f"Sections DataFrame columns: {sections.columns.tolist()}")
    logger.debug(f"Sections DataFrame head:\n{sections.head()}")

    logger.info("Successfully preprocessed profiler data.")
    return sections

def create_bar_chart(sections_df, sort_by='averageTime', ascending=False, top_n=50):
    """Create a horizontal bar chart with min and max times represented as error bars."""
    logger.info(f"Creating bar chart with sort_by='{sort_by}', ascending={ascending}")
    logger.debug(f"DataFrame received:\n{sections_df}")

    if sections_df.empty:
        logger.warning("No data available for bar chart.")
        return px.bar(title='No Data to Display')

    # Remove zero or negative values
    sections_df = sections_df[sections_df[sort_by] > 0]

    if sections_df.empty:
        logger.warning("No data available after removing zero or negative time values.")
        return px.bar(title='No Data to Display')

    # Proceed with sorting
    sorted_df = sections_df.sort_values(by=sort_by, ascending=ascending).head(top_n)
    logger.debug(f"DataFrame after sorting:\n{sorted_df}")

    # Calculate error values
    sorted_df['error_minus'] = sorted_df['averageTime'] - sorted_df['minTime']
    sorted_df['error_plus'] = sorted_df['maxTime'] - sorted_df['averageTime']

    # Ensure no negative errors
    sorted_df['error_minus'] = sorted_df['error_minus'].clip(lower=0)
    sorted_df['error_plus'] = sorted_df['error_plus'].clip(lower=0)

    # Replace any NaN values with zero
    sorted_df['error_minus'] = sorted_df['error_minus'].fillna(0)
    sorted_df['error_plus'] = sorted_df['error_plus'].fillna(0)

    # Create the bar chart with error bars
    fig = px.bar(
        sorted_df,
        x='averageTime',
        y='sectionName',
        orientation='h',
        title=f'Top {top_n} Sections by {sort_by}',
        labels={'averageTime': f'{sort_by} (ms)', 'sectionName': 'Section Name'},
        hover_data=['minTime', 'averageTime', 'maxTime', 'totalTime', 'count'],
        error_x=sorted_df['error_plus'],
        error_x_minus=sorted_df['error_minus']
    )

    # Apply logarithmic scale to the x-axis
    # fig.update_xaxes(type='log')

    # Adjust layout
    fig.update_layout(
        yaxis={'categoryorder': 'total ascending'},
        height=600,
        margin=dict(l=200, r=50, t=50, b=50)
    )

    # Log figure data
    logger.debug(f"Figure data: {fig.data}")

    return fig

def create_gantt_chart(sections_df):
    """Create a Gantt chart for execution timeline visualization."""
    logger.info("Creating Gantt chart for timeline visualization.")

    # Ensure necessary time columns exist
    if not {'startTime', 'endTime'}.issubset(sections_df.columns):
        logger.error("DataFrame must contain 'startTime' and 'endTime' for Gantt chart.")
        return px.timeline(title='Timeline data not available')

    # Convert times to datetime if necessary
    if sections_df['startTime'].dtype != 'datetime64[ns]':
        sections_df['startTime'] = pd.to_datetime(sections_df['startTime'], unit='ms')
    if sections_df['endTime'].dtype != 'datetime64[ns]':
        sections_df['endTime'] = pd.to_datetime(sections_df['endTime'], unit='ms')

    fig = px.timeline(
        sections_df,
        x_start='startTime',
        x_end='endTime',
        y='sectionName',
        color='sectionName',
        hover_data=['averageTime', 'totalTime', 'count']
    )

    fig.update_layout(
        height=600,
        margin=dict(l=200, r=50, t=50, b=50),
        title='Execution Timeline'
    )

    return fig

def generate_insights(sections_df):
    """Generate AI-powered insights using a Hugging Face model."""
    if sections_df.empty:
        logger.warning("No profiling data available for insights.")
        return "No profiling data available."

    # Prepare a detailed summary
    summary = sections_df.to_dict(orient='records')
    logger.info(f"Generating insights with detailed summary.")

    # Create a prompt for the AI model
    prompt = (
        "Given the following profiling data, analyze the performance of the code and suggest optimization strategies.\n\n"
        f"{json.dumps(summary, indent=2)}\n\n"
        "Provide a detailed analysis with specific recommendations."
    )

    try:
        # Load the tokenizer and model
        tokenizer = AutoTokenizer.from_pretrained("google/flan-t5-large")
        model = AutoModelForSeq2SeqLM.from_pretrained("google/flan-t5-large")

        # Move model to GPU if available
        device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
        model = model.to(device)

        # Tokenize the input prompt
        inputs = tokenizer(prompt, return_tensors="pt", max_length=1024, truncation=True).to(device)

        # Generate the response
        outputs = model.generate(
            **inputs,
            max_length=512,
            num_beams=5,
            early_stopping=True
        )

        # Decode and return the response
        insights = tokenizer.decode(outputs[0], skip_special_tokens=True)
        logger.info("Successfully generated insights.")
    except Exception as e:
        logger.error(f"Unexpected error during insights generation: {e}")
        insights = f"An unexpected error occurred: {e}"

    return insights

# Load and preprocess data
profiler_data = load_profiler_data('profiler_stats.json')  # Ensure the correct JSON file path
sections_df = preprocess_data(profiler_data)

# Define app layout
app.layout = dbc.Container([
    # Header
    dbc.Row([
        dbc.Col(html.H1("Profiler Visualization Dashboard"), width=12)
    ], style={'marginTop': 20, 'marginBottom': 20}),

    # Tabs for multiple visualizations
    dbc.Tabs([
        # Tab for Average Time Bar Chart
        dbc.Tab(label='Average Time Bar Chart', children=[
            # Controls: Filter and Sort
            dbc.Row([
                dbc.Col([
                    html.Label("Filter Sections:"),
                    dcc.Input(
                        id='filter-input',
                        type='text',
                        placeholder='Enter section name...',
                        value='',
                        debounce=True,
                        style={'width': '100%'}
                    ),
                ], width=4),
                dbc.Col([
                    html.Label("Sort By:"),
                    dcc.Dropdown(
                        id='sort-dropdown',
                        options=[
                            {'label': 'Average Time', 'value': 'averageTime'},
                            {'label': 'Total Time', 'value': 'totalTime'},
                            {'label': 'Call Count', 'value': 'count'}
                        ],
                        value='averageTime',
                        clearable=False
                    )
                ], width=4),
                dbc.Col([
                    html.Label("Sort Order:"),
                    dcc.Dropdown(
                        id='sort-order-dropdown',
                        options=[
                            {'label': 'Ascending', 'value': 'asc'},
                            {'label': 'Descending', 'value': 'desc'}
                        ],
                        value='desc',
                        clearable=False
                    )
                ], width=4),
            ], style={'marginBottom': 30}),
            # Visualization: Bar Chart
            dbc.Row([
                dbc.Col([
                    dcc.Graph(id='bar-chart')
                ], width=12),
            ]),
            # Automated Insights
            dbc.Row([
                dbc.Col([
                    html.H3("Automated Insights"),
                    html.Pre(id='insights-text', style={'whiteSpace': 'pre-wrap'})
                ], width=12)
            ], style={'marginTop': 30}),
        ]),
        # Tab for Execution Timeline
        dbc.Tab(label='Execution Timeline', children=[
            # Controls: Filter Sections
            dbc.Row([
                dbc.Col([
                    html.Label("Filter Sections:"),
                    dcc.Input(
                        id='timeline-filter-input',
                        type='text',
                        placeholder='Enter section name...',
                        value='',
                        debounce=True,
                        style={'width': '100%'}
                    ),
                ], width=4),
            ], style={'marginBottom': 30}),
            # Visualization: Gantt Chart
            dbc.Row([
                dbc.Col([
                    dcc.Graph(id='timeline-chart')
                ], width=12),
            ]),
        ]),
    ]),

    # Instructions
    dbc.Row([
        dbc.Col([
            html.H3("Instructions"),
            html.P("Use the input boxes to filter sections by name. Adjust the sort options to order the bar chart based on different metrics."),
            html.P("The bar chart displays the average execution time for each profiling section, with error bars representing the min and max times."),
            html.P("The execution timeline shows the start and end times of each section."),
            html.P("Automated insights provide AI-generated analysis to help identify performance bottlenecks and optimization opportunities."),
        ], width=12)
    ], style={'marginTop': 30})
], fluid=True)

# Define callbacks for interactivity
@app.callback(
    [Output('bar-chart', 'figure'),
     Output('insights-text', 'children')],
    [Input('filter-input', 'value'),
     Input('sort-dropdown', 'value'),
     Input('sort-order-dropdown', 'value')]
)
def update_bar_chart(filter_input, sort_by, sort_order):
    """Update the bar chart and AI insights based on user inputs."""
    try:
        logger.info(f"Bar Chart Callback triggered with filter_input='{filter_input}', sort_by='{sort_by}', sort_order='{sort_order}'")

        # Determine sort order
        ascending = True if sort_order == 'asc' else False

        # Filter the DataFrame based on the filter input
        filtered_sections_df = sections_df.copy()
        if filter_input:
            filtered_sections_df = filtered_sections_df[filtered_sections_df['sectionName'].str.contains(filter_input, case=False, na=False)]

        if filtered_sections_df.empty:
            logger.warning("No data available after filtering.")
            fig_bar = px.bar(title='No Data to Display')
            insights = "No profiling data available."
            return fig_bar, insights

        # Create bar chart using the filtered DataFrame
        fig_bar = create_bar_chart(filtered_sections_df, sort_by=sort_by, ascending=ascending)

        # Generate AI-powered insights using the filtered DataFrame
        insights = generate_insights(filtered_sections_df)

        # Log the outputs
        logger.debug(f"Bar chart figure: {fig_bar}")
        logger.debug(f"Insights text: {insights}")

        return fig_bar, insights
    except Exception as e:
        logger.error(f"Bar Chart Callback Error: {e}")
        return dash.no_update, f"An unexpected error occurred: {e}"

@app.callback(
    Output('timeline-chart', 'figure'),
    [Input('timeline-filter-input', 'value')]
)
def update_timeline_chart(filter_input):
    """Update the timeline chart based on user inputs."""
    try:
        logger.info(f"Timeline Chart Callback triggered with filter_input='{filter_input}'")

        # Filter the DataFrame based on the filter input
        filtered_sections_df = sections_df.copy()
        if filter_input:
            filtered_sections_df = filtered_sections_df[filtered_sections_df['sectionName'].str.contains(filter_input, case=False, na=False)]

        if filtered_sections_df.empty:
            logger.warning("No data available after filtering for timeline chart.")
            fig_timeline = px.timeline(title='No Data to Display')
            return fig_timeline

        # Create timeline chart using the filtered DataFrame
        fig_timeline = create_gantt_chart(filtered_sections_df)

        # Log the output
        logger.debug(f"Timeline chart figure: {fig_timeline}")

        return fig_timeline
    except Exception as e:
        logger.error(f"Timeline Chart Callback Error: {e}")
        return dash.no_update

if __name__ == '__main__':
    app.run_server(debug=True)
