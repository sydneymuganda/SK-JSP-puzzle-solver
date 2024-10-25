import streamlit as st
import seaborn as sns
import matplotlib.pyplot as plt
import requests
import numpy as np

st.set_page_config(layout="wide")

def show():
    st.title("Black Scholes Model")
    st.write("This is the Black Scholes Model page.")

    # Move the input fields to the sidebar
    st.sidebar.subheader("Input Parameters")

    # Sidebar input parameters
    S = st.sidebar.number_input("Current Stock Price (S)", min_value=0.0, value=100.0, step=1.0, format="%.2f")
    K = st.sidebar.number_input("Strike Price (K)", min_value=0.0, value=100.0, step=1.0, format="%.2f")
    T = st.sidebar.number_input("Time to Expiration (T) in years", min_value=0.0, value=1.0, step=0.01, format="%.2f")
    r = st.sidebar.number_input("Risk-Free Rate (r)", min_value=0.0, value=0.05, step=0.01, format="%.2f")
    sigma = st.sidebar.number_input("Volatility (σ)", min_value=0.0, value=0.2, step=0.01, format="%.2f")
    
     # Separate section for Heatmap Parameters
    st.sidebar.subheader("Heatmap Parameters")
    put_purchase_price = st.sidebar.number_input("Put Purchase Price", min_value=0.0, value=50.0, step=1.0, format="%.2f")
    call_purchase_price = st.sidebar.number_input("Call Purchase Price", min_value=0.0, value=50.0, step=1.0, format="%.2f")
    stock_range = st.sidebar.slider( "Stock Range for Heatmap", min_value=10, max_value=150, value=(50, 150), step=10)
    volatility_range = st.sidebar.slider("Volatility Range for Heatmap", 0.1, 1.0, (0.1, 0.5), 0.01)
    # Prepare data for sending to backend
    data = {
        "currentPrice": S,
        "strikePrice": K,
        "riskFreeRate": r,
        "volatility": sigma,
        "timeToExpiration": T,
        "callPurchasePrice": call_purchase_price,
        "putPurchasePrice": put_purchase_price
    }

    col1, col2 = st.columns([7, 3])

    with col1:
        st.subheader("Input Parameters")

        st.markdown(f"""
            <div style="display: flex; justify-content: space-around; padding: 10px;">
                <div style="text-align: center;">
                    <strong>Current Stock Price (S)</strong><br>
                    {S}
                </div>
                <div style="text-align: center;">
                    <strong>Strike Price (K)</strong><br>
                    {K}
                </div>
                <div style="text-align: center;">
                    <strong>Time to Expiration (T)</strong><br>
                    {T}
                </div>
                <div style="text-align: center;">
                    <strong>Risk-Free Rate (r)</strong><br>
                    {r}
                </div>
                <div style="text-align: center;">
                    <strong>Volatility (σ)</strong><br>
                    {sigma}
                </div>
                <div style="text-align: center;">
                    <strong>Put Purchase Price</strong><br>
                    {put_purchase_price}
                </div>
                <div style="text-align: center;">
                    <strong>Call Purchase Price</strong><br>
                    {call_purchase_price}
                </div>
            </div>
        """, unsafe_allow_html=True)


    with col2:
        call_price, call_pnl = getOptionPrice(data, "call")
        put_price, put_pnl = getOptionPrice(data, "put")

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
    st.subheader("Options PnL Heat Map")

    col3, col4 = st.columns([5, 5])

    # Simulating PnL data for heatmaps (you can replace this with actual backend response)
    # Parameters
    volatilities = np.linspace(volatility_range[0], volatility_range[1], 10)
    stock_prices = np.linspace(stock_range[0], stock_range[1], 10) 

    # Initialize PnL matrices
    pnl_data_call = np.zeros((len(stock_prices), len(volatilities)))
    pnl_data_put = np.zeros((len(stock_prices), len(volatilities)))

    # Calculate PnL for each stock price and volatility
    for i, stock_price in enumerate(stock_prices):
        for j, volatility in enumerate(volatilities):
            # Simulating current price and using fixed strike price and purchase prices
            pnl_data_call[i, j] = getCallPnl(stock_price, K, call_purchase_price)
            pnl_data_put[i, j] = getPutPnl(stock_price, K, put_purchase_price)

    with col3:
        # Heatmap for Call PnL
        plt.figure(figsize=(10, 8))  # Adjusting size to ensure squares
        sns.heatmap(pnl_data_call, cmap='RdYlGn', annot=True, fmt=".1f", square=True,
                    yticklabels=[f"{vol:.2f}" for vol in volatilities],
                    xticklabels=[f"{price:.2f}" for price in stock_prices], 
                    linecolor='grey', linewidths=0.5)
        plt.title("Call Option PnL Heatmap")
        plt.ylabel("Volatility (σ)")
        plt.xlabel("Stock Price (S)")
        st.pyplot(plt)

    with col4:
        # Heatmap for Put PnL
        plt.figure(figsize=(10, 8))  # Adjusting size to ensure squares
        sns.heatmap(pnl_data_put, cmap='RdYlGn', annot=True, fmt=".1f", square=True,
                    yticklabels=[f"{vol:.2f}" for vol in volatilities],
                    xticklabels=[f"{price:.2f}" for price in stock_prices], 
                    linecolor='grey', linewidths=0.2)
        plt.title("Put Option PnL Heatmap")
        plt.ylabel("Volatility (σ)")
        plt.xlabel("Stock Price (S)")
        st.pyplot(plt)

def getOptionPrice(data, option_type):
    url = 'http://localhost:8092'  # Adjust this to your backend URL
    try:
        response = requests.post(url, json=data)
        response.raise_for_status()

        response_data = response.json()

        if option_type == 'call':
            return response_data['call_price'], response_data['call_pnl']
        elif option_type == 'put':
            return response_data['put_price'], response_data['put_pnl']
        else:
            raise ValueError("Invalid option_type. Expected 'put' or 'call'.")
    except requests.exceptions.RequestException as e:
        st.error(f"An error occurred: {e}")
        return "N/A", "N/A"
    
def getPnl(Stuff):
    if(Stuff):
        return True


def getCallPnl(stock_price, strike_price, purchase_price):
    intrinsicValue = max(0.0, stock_price - strike_price)
    return intrinsicValue - purchase_price


def getPutPnl(stock_price, strike_price, purchase_price):
    intrinsicValue = max(0.0, stock_price - strike_price)
    return intrinsicValue - purchase_price
