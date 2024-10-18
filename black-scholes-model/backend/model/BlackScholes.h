#ifndef BLACKSCHOLES_H
#define BLACKSCHOLES_H

#include <memory>
#include <vector>
#include <string>
#include<functional>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/StreamCopier.h>
#include <Poco/URI.h>
#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/Net/SSLManager.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>
#include <thread> 
#include <chrono> 
#include <iostream>
#include <sstream>
#include <atomic>

class BlackScholes
{
private:
double strikePrice;       
double timeToExpiration;  
double volatility;        
double riskFreeRate;      
double optionPrice;
bool optionType; // if positive call else put.

// Real-time market data: Store live prices fetched from APIs
std::unique_ptr<std::vector<double>> marketPrices; 

// API client for fetching live stock data
std::unique_ptr<std::string> stockSymbol;  
std::string apiKey;

// Numerical settings for precision control
size_t numSteps;          
size_t numSimulations;    

// Helper function to calculate the normal cumulative distribution function
double normalCDF(double value);

public:
    BlackScholes();

     // Constructor with smart pointers and option parameters
    BlackScholes(double strike, double time, double vol, double rate, bool type, 
                      const std::string& symbol, const std::string& api)
        : strikePrice(strike), timeToExpiration(time), volatility(vol), riskFreeRate(rate), optionType(type),
          stockSymbol(std::make_unique<std::string>(symbol)),
          apiKey(api),
          marketPrices(std::make_unique<std::vector<double>>()) {}

    std::string fetchMarketData(const std::string& symbol);
    // Public method to calculate option price
    double calculateOptionPrice();
    double calculateBSFormula(double underlyingPrice, bool type);

    void setStrikePrice(double strike) { strikePrice = strike; }
    double getStrikePrice() const { return strikePrice; }

    void setTimeToEx(double t) { timeToExpiration = t; }
    double getTimeToEx() const { return timeToExpiration; }

    void setStockSymbol(const std::string& symbol) { stockSymbol = std::make_unique<std::string>(symbol); }
    const std::string& getSymbol() const {return *stockSymbol;}

    void setVolatility(double vol) { volatility = vol; }
    double getVolatility() const { return volatility; }

    void setRiskFreeRate(double rate) { riskFreeRate = rate; }
    double getRiskFreeRate() const { return riskFreeRate; }

    bool getOptionType() const { return optionType; }   // to be used
    void setOptionType(bool type) { optionType = type; } // to be used

    double getOptionPrice() const { return optionPrice; }
    void setOptionPrice(double price) { optionPrice = price; }

    void updateWithRealTimeData();
    
    // Concurrency example (simulate Monte Carlo pricing)
    double parallelMonteCarloSim(size_t numSimulations);

    ~BlackScholes();
};

#endif