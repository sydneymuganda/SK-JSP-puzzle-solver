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

using namespace Poco::Net;
using namespace Poco::JSON;
using namespace Poco::Dynamic;
using namespace std;



class MainRequestHandler : public HTTPRequestHandler {
public:
    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) override {
        cout << "Request received on /main endpoint: " << request.getMethod() << endl;

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
        Poco::JSON::Parser parser;
        Var result;
        try {
            result = parser.parse(body);
        } catch (const Poco::Exception& e) {
            response.setStatusAndReason(HTTPResponse::HTTP_BAD_REQUEST);
            response.send() << "Invalid JSON: " << e.displayText();
            return;
        }

        auto jsonObject = result.extract<Poco::JSON::Object::Ptr>();

        // Extract data from the JSON object
        double currentPrice = jsonObject->getValue<double>("currentPrice");
        double strikePrice = jsonObject->getValue<double>("strikePrice");
        double riskFreeRate = jsonObject->getValue<double>("riskFreeRate");
        double volatility = jsonObject->getValue<double>("volatility");
        double timeToExpiration = jsonObject->getValue<double>("timeToExpiration");
        double callPurchasePrice = jsonObject->getValue<double>("callPurchasePrice");
        double putPurchasePrice = jsonObject->getValue<double>("putPurchasePrice");

        BlackScholes bs(strikePrice, timeToExpiration, volatility, riskFreeRate, callPurchasePrice, putPurchasePrice);

        // Calculate the option price
        double put_price = bs.getPutPrice(currentPrice);
        double call_price = bs.getCallPrice(currentPrice);
        double put_pnl = bs.getPutPnl(currentPrice, strikePrice, putPurchasePrice);
        double call_pnl = bs.getCallPnl(currentPrice, strikePrice, callPurchasePrice);

        // Prepare the JSON response
        Poco::JSON::Object::Ptr jsonResponse = new Poco::JSON::Object;
        jsonResponse->set("put_price", put_price);
        jsonResponse->set("call_price", call_price);
        jsonResponse->set("put_pnl", put_pnl);
        jsonResponse->set("call_pnl", call_pnl);

        // Send the JSON response
        response.setContentType("application/json");
        std::ostream& ostr = response.send();
        Poco::JSON::Stringifier::stringify(*jsonResponse, ostr);

        cout << "Response sent on /main endpoint!" << endl;
    }
};