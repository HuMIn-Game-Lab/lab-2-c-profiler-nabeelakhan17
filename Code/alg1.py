import pandas as pd
import plotly.express as px

# Sample data
data = [
    {'sectionName': 'Trig Speed Test', 'averageTime': 27.8065},
    {'sectionName': 'Random Angle Generation', 'averageTime': 0.723859},
    {'sectionName': 'Total Cos and Sin Compute', 'averageTime': 26.9776},
    {'sectionName': 'Cos and Sine Compute', 'averageTime': 0.0000808211},
    {'sectionName': 'Task A', 'averageTime': 0.008162},
    {'sectionName': 'Task B', 'averageTime': 0.0001054}
]

df = pd.DataFrame(data)

# Remove zero or negative values
df = df[df['averageTime'] > 0]

# Create the bar chart
fig = px.bar(
    df,
    x='averageTime',
    y='sectionName',
    orientation='h',
    title='Test Bar Chart',
    labels={'averageTime': 'Average Time (ms)', 'sectionName': 'Section Name'}
)

# Apply logarithmic scale to the x-axis
fig.update_xaxes(type='log')

fig.show()
