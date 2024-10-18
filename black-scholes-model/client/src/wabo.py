import requests
import json

# Define the server URL
url = 'http://localhost:8090'

# Prepare the data you want to send as JSON
data = {
    "currentPrice": 143,
    "strikePrice": 89.00,
    "riskFreeRate": 0.01,
    "volatility": 0.9,
    "timeToExpiration": 0.4,
    "isCallOption": True
}

# Send a POST request to the server
try:
    response = requests.post(url, json=data)
    
    # Check if the request was successful
    response.raise_for_status()  # Raises an error for 4xx or 5xx responses

    # Parse the JSON response
    response_data = response.json()
    
    # Print the received option price
    print("Received option price:", response_data.get("optionPrice"))

except requests.exceptions.RequestException as e:
    print("An error occurred:", e)