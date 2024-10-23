import streamlit as st
import pandas as pd
import plotly.express as px
import os

# Set Streamlit page configuration
st.set_page_config(page_title="C++ Profiler Elapsed Time Dashboard", layout="wide")

# Title of the dashboard
st.title("📊 C++ Profiler Elapsed Time Dashboard")

# Sidebar for filtering
st.sidebar.header("Filter Options")

# Function to load CSV data
@st.cache_data
def load_data(file_path):
    if not os.path.exists(file_path):
        st.error(f"Error: The file '{file_path}' does not exist.")
        return None
    try:
        df = pd.read_csv(file_path)
        # Convert relevant columns to numeric
        numeric_cols = ['elapsedTime(ms)', 'lineNumber']
        df[numeric_cols] = df[numeric_cols].apply(pd.to_numeric, errors='coerce')
        # Fill missing values
        df.fillna({'fileName': 'N/A', 'functionName': 'N/A'}, inplace=True)
        return df
    except Exception as e:
        st.error(f"Error loading CSV file: {e}")
        return None

# Load data
csv_file = 'Data/detailedStats.csv'  # Ensure this file is in the same directory as app.py
df = load_data(csv_file)

if df is not None:
    # Sidebar - Filter by sectionName
    section_names = df['sectionName'].unique().tolist()
    selected_sections = st.sidebar.multiselect(
        "Select Section(s) to Display",
        options=section_names,
        default=section_names  # Select all by default
    )

    if selected_sections:
        # Filter data based on selected sections
        filtered_df = df[df['sectionName'].isin(selected_sections)]
        
        # Ensure separate counts for each section
        filtered_df['Count'] = filtered_df.groupby('sectionName').cumcount() + 1  # Reset count for each section

        # Plot: Elapsed Time Line Graph with different lines for each section
        st.subheader("Elapsed Time (ms) by Count and Section")
        fig = px.line(
            filtered_df,
            x='Count',  # x-axis is the count
            y='elapsedTime(ms)',  # y-axis is the elapsed time
            color='sectionName',  # Different colored lines for each section
            labels={
                'Count': 'Count',
                'elapsedTime(ms)': 'Elapsed Time (ms)',
                'sectionName': 'Section Name'
            },
            title='Elapsed Time (ms) by Count and Section',
            markers=True
        )
        fig.update_layout(
            xaxis_title='Count',
            yaxis_title='Elapsed Time (ms)',
        )
        st.plotly_chart(fig, use_container_width=True)

        # Display the filtered data (no startTime or stopTime)
        st.subheader("Data Table")
        st.dataframe(filtered_df[['Count', 'sectionName', 'elapsedTime(ms)', 'lineNumber', 'functionName']])

        # Optional: Download the filtered data
        csv_filtered = filtered_df.to_csv(index=False).encode('utf-8')
        st.download_button(
            label="Download Filtered Data as CSV",
            data=csv_filtered,
            file_name='filtered_profiler_stats.csv',
            mime='text/csv',
        )
    else:
        # Display a message when no sections are selected
        st.warning("Please select at least one section to display the graph.")
else:
    st.error("No data loaded. Please check the CSV file path.")
