import pandas as pd
import requests
import time

# Load the CSV file
file_path = r"" # Insert location of file on computer 
data = pd.read_csv(file_path)

# Define the endpoint
endpoint = "https://biome-iot.uqcloud.net/api/monitoring_data"

# Prepare the data to be sent in batches
example_data = []
sensor_id = 1
sensor_name = "Dry test - multiple days" # Name of graph shown on internet dashboard

batch_size = 20  # Number of records per batch
total_records = len(data)
batches_sent = 0

for index, row in data.iterrows():
    example_data.append({
        "timestamp": row['Timestamp (ms)'],
        "sensor_id": sensor_id,
        "sensor_name": sensor_name,
        "sensor_value": row['Voltage (mV)']
    })
    
    # Send data within batches specified 
    if len(example_data) >= batch_size:
        response = requests.post(endpoint, json=example_data)
        batches_sent += 1
        print(f"Batch {batches_sent} sent with status code: {response.status_code}")
        print(f"Progress: {min(index + 1, total_records)}/{total_records} records processed")
        example_data = []  # Clear the list after sending a batch
        time.sleep(1)  # Pause to avoid overwhelming the server

# Send any remaining data
if example_data:
    response = requests.post(endpoint, json=example_data)
    batches_sent += 1
    print(f"Final batch {batches_sent} sent with status code: {response.status_code}")
    print(f"Progress: {total_records}/{total_records} records processed")
