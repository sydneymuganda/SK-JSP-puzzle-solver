import streamlit as st
import matplotlib as plt
import requests
import json
import streamlit as st
import requests
import matplotlib.pyplot as plt
import numpy as np
st.set_page_config(layout="wide")
def show():
    st.title("Black Scholes Model")
    st.write("This is the Black Scholes Model page.")

    col1, col2 = st.columns([7, 3]) 

    with col1:
        st.subheader("Input Parameters")

        # Create a horizontal layout for input parameters
        inputs = st.columns(6)  # Create 6 equal-width columns for inputs

        # Input parameters
        S = inputs[0].number_input("Current Stock Price (S)", min_value=0.0, value=100.0, step=1.0, format="%.2f")
        K = inputs[1].number_input("Strike Price (K)", min_value=0.0, value=100.0, step=1.0, format="%.2f")
        T = inputs[2].number_input("Time to Expiration (T) in years", min_value=0.0, value=1.0, step=0.01, format="%.2f")
        r = inputs[3].number_input("Risk-Free Rate (r)", min_value=0.0, value=0.05, step=0.01, format="%.2f")
        sigma = inputs[4].number_input("Volatility (σ)", min_value=0.0, value=0.2, step=0.01, format="%.2f")
        
        # Prepare data for sending to backend
        data = {
            "currentPrice": S,
            "strikePrice": K,
            "riskFreeRate": r,
            "volatility": sigma,
            "timeToExpiration": T,
            "isCallOption": True
        }
        
            
    with col2:

        call_price =  getOptionPrice(data) 
        put_price =   getOptionPrice(data)
        # Display the option prices in smaller rounded boxes
        st.markdown(
            f"""
            <div style="display: flex; justify-content: center; margin-top: 70px; gap: 10px;">
                <div style="background-color: #e0e0e0; border-radius: 10px; padding: 10px; width: 90%;">
                    <div style="text-align: center; font-size: 1em; font-weight: bold;">Call Price</div>
                    <div style="text-align: center; color: #333; font-size: 1.2em;">${call_price}</div>
                </div>
                <div style="background-color: #e0e0e0; border-radius: 10px; padding: 10px; width: 80%;">
                    <div style="text-align: center; font-size: 1em; font-weight: bold;">Put Price</div>
                    <div style="text-align: center; color: #333; font-size: 1.2em;">${put_price}</div>
                </div>
            </div>
            """,
            unsafe_allow_html=True
        )

    # Visualization below the two columns
    st.subheader("Visualizations")

    # Create a plot of option prices against different strikes (placeholder)
    # fig, ax = plt.subplots()
    # strike_prices = np.linspace(50, 150, 100)
    
    # # Dummy data for visualization (replace with actual option price data)
    # dummy_prices = np.random.uniform(low=5, high=25, size=strike_prices.shape)  
    # ax.plot(strike_prices, dummy_prices, label='Option Price', color='blue')
    # ax.axhline(y=0, color='black', linestyle='--')
    # ax.set_title('Option Price vs. Strike Price')
    # ax.set_xlabel('Strike Price')
    # ax.set_ylabel('Option Price')
    # ax.legend()
    
    # st.pyplot(fig)

def getOptionPrice(data):
    url = 'http://localhost:8091'  # Adjust this to your backend URL
    # Send a POST request to the server
    try:
        response = requests.post(url, json=data)
        
        # Check if the request was successful
        response.raise_for_status()  # Raises an error for 4xx or 5xx responses

        # Parse the JSON response
        response_data = response.json()
        
        
        # Print the received option price
        optP = response_data.get("optionPrice", 0.0)

        return float(optP)

    except requests.exceptions.RequestException as e:
        st.error(f"An error occurred: {e}")
        return "N/A", "N/A"  # Return placeholders if there is an error