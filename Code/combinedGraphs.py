import streamlit as st
import pandas as pd
import plotly.express as px
import os

# Set Streamlit page configuration
st.set_page_config(page_title="C++ Profiler Dashboard", layout="wide")

# ----------------- First Graph (Elapsed Time) -----------------
st.title("📊 C++ Profiler Elapsed Time Dashboard")

# Function to load CSV data and rename columns for consistency
@st.cache_data
def load_data(file_path, rename_dict=None):
    if not os.path.exists(file_path):
        st.error(f"Error: The file '{file_path}' does not exist.")
        return None
    try:
        df = pd.read_csv(file_path)
        # Rename columns if a rename dictionary is provided
        if rename_dict:
            df.rename(columns=rename_dict, inplace=True)
        return df
    except Exception as e:
        st.error(f"Error loading CSV file: {e}")
        return None

# ------------------- Graph 1: Elapsed Time ---------------------
# Load data for the first graph (Elapsed Time)
csv_file_1 = 'Data/detailedStats.csv'  # Replace with your file path for first dataset
df1 = load_data(csv_file_1)

if df1 is not None:
    st.markdown("### Graph 1: Elapsed Time (ms) by Section")
    # Sidebar filter for first graph
    st.sidebar.header("Filter Options for Elapsed Time Graph")
    section_names_1 = df1['sectionName'].unique().tolist()
    selected_sections_1 = st.sidebar.multiselect(
        "Select Section(s) for Elapsed Time Graph",
        options=section_names_1,
        default=section_names_1  # Select all by default
    )

    if selected_sections_1:
        # Filter data based on selected sections
        filtered_df1 = df1[df1['sectionName'].isin(selected_sections_1)]
        filtered_df1['Count'] = filtered_df1.groupby('sectionName').cumcount() + 1  # Add count per section

        # Plot: Elapsed Time Line Graph for Dataset 1
        fig1 = px.line(
            filtered_df1,
            x='Count',
            y='elapsedTime(ms)',
            color='sectionName',
            title='Elapsed Time (ms) by Count and Section',
            markers=True
        )
        fig1.update_layout(
            xaxis_title='Count',
            yaxis_title='Elapsed Time (ms)',
        )
        st.plotly_chart(fig1, use_container_width=True)

        # Display filtered data for the first graph
        st.subheader("Data Table for Elapsed Time Graph")
        st.dataframe(filtered_df1[['Count', 'sectionName', 'elapsedTime(ms)', 'lineNumber', 'functionName']])

        # Download option for the first graph
        csv_filtered_1 = filtered_df1.to_csv(index=False).encode('utf-8')
        st.download_button(
            label="Download Filtered Data for Elapsed Time Graph as CSV",
            data=csv_filtered_1,
            file_name='filtered_profiler_stats_elapsed_time.csv',
            mime='text/csv',
        )
    else:
        st.warning("Please select at least one section to display for the Elapsed Time graph.")
else:
    st.error("No data loaded for Elapsed Time graph. Please check the CSV file path.")


# ----------------- Second Graph (Average Execution Time) -----------------
st.markdown("### Graph 2: Average Execution Time per Section")

# Column renaming dictionary for second dataset
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

# Load data for the second graph (Average Execution Time)
csv_file_2 = 'Data/profiler_stats.csv'  # Replace with your file path for second dataset
df2 = load_data(csv_file_2, rename_dict=rename_dict)

if df2 is not None:
    st.sidebar.header("Filter Options for Average Time Graph")
    # Sidebar filter for second graph
    section_names_2 = df2['sectionName'].unique().tolist()
    selected_sections_2 = st.sidebar.multiselect(
        "Select Section(s) for Average Time Graph",
        options=section_names_2,
        default=section_names_2  # Select all by default
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
    if selected_sections_2:
        filtered_df2 = df2[df2['sectionName'].isin(selected_sections_2)]
        # Ensure the selected statistic exists before sorting
        if statistic in filtered_df2.columns:
            sorted_df = filtered_df2.sort_values(by=statistic, ascending=ascending)

            # Plot the bar chart for the second graph
            fig2 = px.bar(
                sorted_df,
                x='sectionName',
                y=statistic,
                title=f'{statistic} per Section (ms)',
                labels={statistic: statistic, 'sectionName': 'Section Name'},
                hover_data=['count', 'totalTime_ms'],
                color=statistic,
                color_continuous_scale='Viridis'
            )
            fig2.update_layout(
                xaxis_tickangle=-45,
                xaxis={'categoryorder': 'array', 'categoryarray': sorted_df['sectionName']}
            )
            st.plotly_chart(fig2, use_container_width=True)

            # Display the filtered data for the second graph
            st.subheader("Data Table for Average Execution Time Graph")
            st.dataframe(sorted_df[['sectionName', statistic, 'count', 'totalTime_ms', 'functionName']])

            # Download option for the second graph
            csv_filtered_2 = sorted_df.to_csv(index=False).encode('utf-8')
            st.download_button(
                label="Download Filtered Data for Average Time Graph as CSV",
                data=csv_filtered_2,
                file_name='filtered_profiler_stats_average_time.csv',
                mime='text/csv',
            )
        else:
            st.error(f"The selected statistic '{statistic}' is not available in the dataset.")
    else:
        st.warning("Please select at least one section to display for the Average Time graph.")
else:
    st.error("No data loaded for Average Execution Time graph. Please check the CSV file path.")
