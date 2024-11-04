#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/JSON/Parser.h>
#include <Poco/JSON/Stringifier.h>
#include <Poco/StreamCopier.h>
#include <Poco/JSON/Object.h>
#include <Poco/Dynamic/Var.h>
#include <iostream>
#include "../model/BlackScholes.h" // Ensure this path is correct
#include <vector>
#include <array>

using namespace Poco::Net;
using namespace Poco::JSON;
using namespace Poco::Dynamic;
using namespace std;

class HeatmapRequestHandler : public HTTPRequestHandler {
public:
    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) override {
        cout << "Request received on /heatmap endpoint: " << request.getMethod() << endl;

        // Check if the request method is POST
        if (request.getMethod() != "POST") {
            response.setStatusAndReason(HTTPResponse::HTTP_BAD_REQUEST);
            response.send() << "Only POST requests are allowed.";
            return;
        }

        // Read the request body
        std::istream& istr = request.stream();
        std::string body;
        Poco::StreamCopier::copyToString(istr, body);

        // Parse the JSON data
        Parser parser;
        Var result;
        try {
            result = parser.parse(body);
        } catch (const Poco::Exception& e) {
            response.setStatusAndReason(HTTPResponse::HTTP_BAD_REQUEST);
            response.send() << "Invalid JSON: " << e.displayText();
            return;
        }

        auto jsonObject = result.extract<Object::Ptr>();

        // Extract parameters from JSON
        double K = jsonObject->getValue<double>("strikePrice");
        double call_purchase_price = jsonObject->getValue<double>("callPurchasePrice");
        double put_purchase_price = jsonObject->getValue<double>("putPurchasePrice");
        
        // Extract ranges for stock prices and volatilities
        Poco::JSON::Array::Ptr stockRangeArray = jsonObject->getArray("stockRange");
        Poco::JSON::Array::Ptr volatilityRangeArray = jsonObject->getArray("volatilityRange");

        std::array<double, 2> stock_range = {stockRangeArray->getElement<double>(0), stockRangeArray->getElement<double>(1)};
        std::array<double, 2> volatility_range = {volatilityRangeArray->getElement<double>(0), volatilityRangeArray->getElement<double>(1)};

        // Simulating PnL data for heatmaps
        // Parameters
        auto volatilities = linspace(volatility_range[0], volatility_range[1], 10);
        auto stock_prices = linspace(stock_range[0], stock_range[1], 10); 

        // Initialize PnL matrices
        vector<vector<double>> pnl_data_call(stock_prices.size(), vector<double>(volatilities.size(), 0.0));
        vector<vector<double>> pnl_data_put(stock_prices.size(), vector<double>(volatilities.size(), 0.0));

        // Calculate PnL for each stock price and volatility
        for (size_t i = 0; i < stock_prices.size(); ++i) {
            for (size_t j = 0; j < volatilities.size(); ++j) {
                double stock_price = stock_prices[i];
                double volatility = volatilities[j];

                // Using the Black-Scholes model to calculate PnL
                BlackScholes bs(K, 1.0, volatility, 0.05, call_purchase_price, put_purchase_price); // Assuming time to expiration = 1 year and risk-free rate = 0.05

                pnl_data_call[i][j] = bs.getCallPnl(stock_price, K, call_purchase_price);
                pnl_data_put[i][j] = bs.getPutPnl(stock_price, K, put_purchase_price);
            }
        }

        // Prepare the JSON response
        Object::Ptr jsonResponse = new Object;
        jsonResponse->set("pnl_data_call", toJsonArray(pnl_data_call));
        jsonResponse->set("pnl_data_put", toJsonArray(pnl_data_put));

        // Send the JSON response
        response.setContentType("application/json");
        std::ostream& ostr = response.send();
        Stringifier::stringify(*jsonResponse, ostr);

        cout << "Response sent on /heatmap endpoint!" << endl;
    }

private:
    // Helper function to create a JSON array from a 2D vector
    Poco::JSON::Array::Ptr toJsonArray(const std::vector<std::vector<double>>& data) {
        Poco::JSON::Array::Ptr jsonArray = new Poco::JSON::Array;
        for (const auto& row : data) {
            Poco::JSON::Array::Ptr jsonRow = new Poco::JSON::Array;
            for (double value : row) {
                jsonRow->add(value);
            }
            jsonArray->add(jsonRow);
        }
        return jsonArray;
    }
    // Helper function to generate linearly spaced values
    vector<double> linspace(double start, double end, size_t num) {
        vector<double> values(num);
        double step = (end - start) / (num - 1);
        for (size_t i = 0; i < num; ++i) {
            values[i] = start + i * step;
        }
        return values;
    }
};