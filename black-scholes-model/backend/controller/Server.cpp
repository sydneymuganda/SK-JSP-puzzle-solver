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
#include <iostream>
#include "../model/BlackScholes.h" 
#include "MainRequestHandler.cpp"
#include "HeatmapRequestHandler.cpp"

using namespace Poco::Net;
using namespace Poco::Dynamic;
using namespace Poco::Util; // Add this to use Application and ServerApplication
using namespace std;

// RequestHandlerFactory to produce RequestHandlers
class RequestHandlerFactory : public HTTPRequestHandlerFactory {
public:
    HTTPRequestHandler* createRequestHandler(const HTTPServerRequest& request) override {
        const std::string& uri = request.getURI();
        if (uri == "/main") {
            return new MainRequestHandler;
        } else if (uri == "/heatmap") {
            return new HeatmapRequestHandler;
        } else {
            return nullptr; // Return nullptr to handle undefined endpoints elsewhere
        }
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

        ServerSocket svs(8092); // Listening on port 8092
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