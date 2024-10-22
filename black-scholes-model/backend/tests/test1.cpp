// #include <Poco/Net/HTTPServer.h>
// #include <Poco/Net/HTTPRequestHandler.h>
// #include <Poco/Net/HTTPRequestHandlerFactory.h>
// #include <Poco/Net/HTTPServerRequest.h>
// #include <Poco/Net/HTTPServerResponse.h>
// #include <Poco/Net/ServerSocket.h>
// #include <Poco/JSON/Parser.h>
// #include <Poco/JSON/Stringifier.h>
// #include <Poco/StreamCopier.h>
// #include <Poco/JSON/Object.h>
// #include <Poco/Dynamic/Var.h>
// #include <Poco/Util/ServerApplication.h>
// #include <Poco/Util/Application.h>
// #include <iostream>
// #include <thread>
// #include "./model/BlackScholes.h"

// using namespace std;


// void testJsonParsing() {
//     std::string jsonRequest = R"({
//         "currentPrice": 143,
//         "strikePrice": 89.00,
//         "riskFreeRate": 0.01,
//         "volatility": 0.9,
//         "timeToExpiration": 0.4,
//         "isCallOption": true
//     })";

//     Poco::JSON::Parser parser;
//     try {
//         Poco::Dynamic::Var result = parser.parse(jsonRequest);
//         auto jsonObject = result.extract<Poco::JSON::Object::Ptr>();

//         // Check the values
//         std::cout << "Current Price: " << jsonObject->getValue<double>("currentPrice") << std::endl;
//         std::cout << "Strike Price: " << jsonObject->getValue<double>("strikePrice") << std::endl;
//         std::cout << "Risk Free Rate: " << jsonObject->getValue<double>("riskFreeRate") << std::endl;
//         std::cout << "Volatility: " << jsonObject->getValue<double>("volatility") << std::endl;
//         std::cout << "Time to Expiration: " << jsonObject->getValue<double>("timeToExpiration") << std::endl;
//         std::cout << "Is Call Option: " << jsonObject->getValue<bool>("isCallOption") << std::endl;
//     } catch (const Poco::Exception& e) {
//         std::cerr << "Error parsing JSON: " << e.displayText() << std::endl;
//     }
// }

// void testBlackScholesCalculation() {
//     double currentPrice = 143;
//     double strikePrice = 89.00;
//     double riskFreeRate = 0.01;
//     double volatility = 0.9;
//     double timeToExpiration = 0.4;

//     BlackScholes bs;
//     double callPrice = bs.calculateBSFormula(currentPrice, true); // Call option
//     double putPrice = bs.calculateBSFormula(currentPrice, false); // Put option

//     std::cout << "Call Option Price: " << callPrice << std::endl;
//     std::cout << "Put Option Price: " << putPrice << std::endl;
// }
