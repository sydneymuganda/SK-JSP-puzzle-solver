#include "BlackScholes.h"
#include "../services/timer.cpp"
#include <future>

using namespace std;
std::atomic<bool> keepUpdating(true); 
BlackScholes::BlackScholes(){}

double BlackScholes::normalCDF(double value){
    return 0.5 * std::erfc(-value / std::sqrt(2));
}

double BlackScholes::calculateBSFormula(double currentPrice, bool isCallOption) {
    // Variables required for Black-Scholes formula
    double d1 = (log(currentPrice / strikePrice) + (riskFreeRate + (volatility * volatility) / 2) * timeToExpiration) / (volatility * sqrt(timeToExpiration));
    double d2 = d1 - volatility * sqrt(timeToExpiration);

    // CDF of normal distribution
    double N_d1 = normalCDF(d1);
    double N_d2 = normalCDF(d2);

    // Calculate call price
    if (isCallOption) {
        return currentPrice * N_d1 - strikePrice * exp(-riskFreeRate * timeToExpiration) * N_d2;
    }
    // Calculate put price
    else {
        return strikePrice * exp(-riskFreeRate * timeToExpiration) * (1 - N_d2) - currentPrice * (1 - N_d1);
    }
}

void BlackScholes::updateWithRealTimeData() {

    while (keepUpdating) {
        std::string symbol = getSymbol();
        std::string jsonData = fetchMarketData(symbol);  // Fetch market data (JSON)
        std::cout << "Full JSON response: " << jsonData << std::endl;
        bool optionType;

        // Parse JSON response using Poco JSON parser
        Poco::JSON::Parser parser;
        try {
            Poco::Dynamic::Var result = parser.parse(jsonData);
            Poco::JSON::Object::Ptr obj = result.extract<Poco::JSON::Object::Ptr>();

            // Check if the "data" key exists in the JSON response
            if (obj->has("data")) {
                Poco::JSON::Array::Ptr dataArray = obj->getArray("data");

                if (dataArray && !dataArray->empty()) {
                    // Get the first option (or iterate over all if needed)
                    Poco::JSON::Object::Ptr firstOption = dataArray->getObject(0);

                    // Serialize the JSON object to a string
                    std::stringstream ss;
                    firstOption->stringify(ss);  // Serialize JSON to string
                    std::cout << "First option data: " << ss.str() << std::endl;

                    // Determine if this is a call or put option
                    std::string optionTypeStr = firstOption->getValue<std::string>("type");
                    optionType = (optionTypeStr == "call");  // true for call, false for put

                    // Get relevant data from the JSON object
                    double strikePrice = firstOption->getValue<double>("strike");
                    double impliedVolatility = firstOption->getValue<double>("implied_volatility") / 100.0;  // Assuming percentage
                    double currentPrice = firstOption->getValue<double>("mark");
                    double timeToMaturity = calculateTime(firstOption->getValue<std::string>("expiration"));

                    // Update the Black-Scholes object based on real-time data
                    setStrikePrice(strikePrice);
                    setVolatility(impliedVolatility);
                    setTimeToEx(timeToMaturity);

                    // Calculate and output the option price
                    std::cout << "Current mark price for " << symbol << ": " << currentPrice << std::endl;
                    std::cout << (optionType ? "Call" : "Put") << " option price: " << calculateBSFormula(currentPrice, optionType) << std::endl;
                } else {
                    std::cerr << "Data array is empty or not found." << std::endl;
                }
            } else {
                std::cerr << "Key 'data' not found in the JSON response." << std::endl;
            }
        } catch (const Poco::Exception& ex) {
            std::cerr << "Error parsing JSON: " << ex.displayText() << std::endl;
        }

        // Sleep for 5 minutes before the next update
        std::this_thread::sleep_for(std::chrono::minutes(10));  // Non-blocking other parts of the system
    }
}

std::string BlackScholes::fetchMarketData(const std::string& symbol) {
    // Create a Poco URI for the API endpoint
    Poco::URI uri("https://www.alphavantage.co/query");
    uri.addQueryParameter("function", "HISTORICAL_OPTIONS");
    uri.addQueryParameter("symbol", symbol);
    uri.addQueryParameter("interval", "10min");
    uri.addQueryParameter("apikey", apiKey);

    // Create an HTTPS session (since we're using HTTPS)
    Poco::Net::HTTPSClientSession session(uri.getHost(), uri.getPort());

    // Create the HTTP GET request
    Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, uri.getPathAndQuery(), Poco::Net::HTTPRequest::HTTP_1_1);
    
    // Send the request
    session.sendRequest(request);

    // Get the response
    Poco::Net::HTTPResponse response;
    std::istream& res = session.receiveResponse(response);

    // If the response status is OK, process the result
    if (response.getStatus() == Poco::Net::HTTPResponse::HTTP_OK) {
        std::ostringstream oss;
        Poco::StreamCopier::copyStream(res, oss);
        return oss.str();  // Return the response as a string
    } else {
        std::cerr << "Error fetching data: " << response.getStatus() << " " << response.getReason() << std::endl;
        return "";
    }
}


// Main function to calculate option price (if real-time data is not used)
// input data from the streamlit app into this function
double BlackScholes::calculateOptionPrice() {
    double underlyingPrice = 100.0;
    optionPrice = calculateBSFormula(underlyingPrice, optionType); 
    return optionPrice;
}

double BlackScholes::parallelMonteCarloSim(size_t numSimulations) {
    std::vector<std::future<double>> futures;

    for (size_t i = 0; i < numSimulations; ++i)
    {
        futures.push_back(std::async(std::launch::async, [this](){
            double price = calculateBSFormula(100.0, optionType);
            return price;
        }));
    }

    // Collect results from all simulations
    double total = 0.0;
    for (auto &fut : futures)
    {
        total += fut.get();
         
    }
    
    // Return the average of all simulations
    return total / numSimulations;
}

BlackScholes::~BlackScholes(){}

