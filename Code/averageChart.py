import streamlit as st
import pandas as pd
import plotly.express as px
import os

# Set Streamlit page configuration
st.set_page_config(page_title="Average Execution Time Dashboard", layout="wide")

# Title of the dashboard
st.title("📊 Average Execution Time per Section")

# Sidebar for filtering and sorting
st.sidebar.header("Filter and Sort Options")

# Function to load CSV data
@st.cache_data
def load_data(file_path):
    if not os.path.exists(file_path):
        st.error(f"Error: The file '{file_path}' does not exist.")
        return None
    try:
        df = pd.read_csv(file_path)
        # Rename columns for consistency
        rename_dict = {
            'totalTime(min)': 'totalTime_ms',
            'minTime(min)': 'minTime_ms',
            'maxTime(min)': 'maxTime_ms',
            'averageTime(min)': 'averageTime_ms',
            'totalTime(ms)': 'totalTime_ms',
            'minTime(ms)': 'minTime_ms',
            'maxTime(ms)': 'maxTime_ms',
            'averageTime(ms)': 'averageTime_ms'
        }
        df.rename(columns=rename_dict, inplace=True)
        # Convert relevant columns to numeric
        numeric_cols = ['count', 'totalTime_ms', 'minTime_ms', 'maxTime_ms', 'averageTime_ms', 'lineNumber']
        for col in numeric_cols:
            if col in df.columns:
                df[col] = pd.to_numeric(df[col], errors='coerce')
        # Fill missing values
        df.fillna({'fileName': 'N/A', 'functionName': 'N/A'}, inplace=True)
        return df
    except Exception as e:
        st.error(f"Error loading CSV file: {e}")
        return None

# Load data
csv_file = 'Data/profiler_stats.csv'
df = load_data(csv_file)

if df is not None:
    # Sidebar - Filter by sectionName
    section_names = df['sectionName'].unique().tolist()
    selected_sections = st.sidebar.multiselect(
        "Select Section(s) to Display",
        options=section_names,
        default=section_names  # Select all by default
    )
    
    # Sidebar - Select which statistic to display
    statistic = st.sidebar.selectbox(
        "Select Statistic to Display",
        options=['totalTime_ms', 'minTime_ms', 'maxTime_ms', 'averageTime_ms'],
        index=3  # Default to 'averageTime_ms'
    )
    
    # Sidebar - Sort order
    sort_order = st.sidebar.selectbox(
        "Sort Order",
        options=["Descending", "Ascending"],
        index=0
    )
    ascending = True if sort_order == "Ascending" else False
    
    # Filter data based on selected sections
    if selected_sections:
        filtered_df = df[df['sectionName'].isin(selected_sections)]
    else:
        filtered_df = df.copy()
    
    # Sort the data based on the selected statistic
    sorted_df = filtered_df.sort_values(by=statistic, ascending=ascending)
    
    # Plot the bar chart
    fig = px.bar(
        sorted_df,
        x='sectionName',
        y=statistic,
        title=f'{statistic} per Section (ms)',
        labels={statistic: statistic, 'sectionName': 'Section Name'},
        hover_data=['count', 'totalTime_ms'],
        color=statistic,
        color_continuous_scale='Viridis'
    )
    
    # Update layout to respect the sorted order
    fig.update_layout(
        xaxis_tickangle=-45,
        xaxis={'categoryorder': 'array', 'categoryarray': sorted_df['sectionName']}
    )
    
    st.plotly_chart(fig, use_container_width=True)
    
    # Optionally, display the filtered data
    st.subheader("Data Table")
    st.dataframe(sorted_df[['sectionName', statistic, 'count', 'totalTime_ms', 'functionName']])
    
    # Optionally, download the filtered data
    csv_filtered = sorted_df.to_csv(index=False).encode('utf-8')
    st.download_button(
        label="Download Filtered Data as CSV",
        data=csv_filtered,
        file_name='filtered_profiler_stats.csv',
        mime='text/csv',
    )
else:
    st.error("No data loaded. Please check the CSV file path.")
