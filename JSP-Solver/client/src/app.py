import streamlit as st
import black_scholes
import volatility_forecasting
import montecarlo_simulation

st.markdown("""
    <style>
    
    .css-1d391kg.e1fqkh3o1 {  
        max-width: 35% !important;  
    }
    .stButton button {
        width: 100%;  /* Full width responsive buttons */
        margin-bottom: 10px;  /* Add some space between buttons */
    }
    </style>
    """, unsafe_allow_html=True)

# Sidebar with full-sized buttons
st.sidebar.title("Navigation")

# Define which page is currently active
if 'page' not in st.session_state:
    st.session_state.page = 'Black Scholes Model'  # Set default page

# Buttons in the sidebar
if st.sidebar.button("Black Scholes Model"):
    st.session_state.page = 'Black Scholes Model'

if st.sidebar.button("Volatility Forecasting"):
    st.session_state.page = 'Volatility Forecasting'

if st.sidebar.button("MonteCarlo Simulation"):
    st.session_state.page = 'MonteCarlo Simulation'


# Route to the selected page
if st.session_state.page == 'Black Scholes Model':
    black_scholes.show()
elif st.session_state.page == 'Volatility Forecasting':
    volatility_forecasting.show()
elif st.session_state.page == 'MonteCarlo Simulation':
    montecarlo_simulation.show()