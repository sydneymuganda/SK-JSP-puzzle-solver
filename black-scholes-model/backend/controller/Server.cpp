#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/JSON/Parser.h>
#include <Poco/JSON/Stringifier.h>
#include <Poco/StreamCopier.h>
#include <Poco/JSON/Object.h>
#include <Poco/Dynamic/Var.h>
#include <Poco/Util/ServerApplication.h>
#include <Poco/Util/Application.h>
#include <iostream>
#include <thread>
#include "../model/BlackScholes.h" // Ensure this path is correct

using namespace Poco::Net;
using namespace Poco::Dynamic;
using namespace Poco::Util; // Add this to use Application and ServerApplication
using namespace std;

class RequestHandler : public HTTPRequestHandler {
public:
    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) override {
        cout << "Request received: " << request.getMethod() << endl;

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
        double call_pnl= bs.getCallPnl(currentPrice, strikePrice, callPurchasePrice);

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

        cout << "Response sent !!! "<< endl;
    }

 
};

// RequestHandlerFactory to produce RequestHandlers
class RequestHandlerFactory : public HTTPRequestHandlerFactory {
public:
    HTTPRequestHandler* createRequestHandler(const HTTPServerRequest& request) override {
        return new RequestHandler;
    }
};

// Main application class
class ScholesBackend : public Poco::Util::ServerApplication {
public:
    void startServer() {
        // Configure the server parameters
        auto params = new HTTPServerParams;
        params->setMaxThreads(16); // Set the maximum number of threads to handle requests
        params->setMaxQueued(100); // Set maximum queued connections

        ServerSocket svs(8092); // Listening on port 8090
        HTTPServer srv(new RequestHandlerFactory, svs, params);

        srv.start();
        cout << "Server started on port 8092...." << endl;

        waitForTerminationRequest();  // Wait for CTRL+C or kill command

        cout << "Shutting down..." << endl;
        srv.stop();
    }
};

void runServer() {
    ScholesBackend app;
    app.startServer();
}