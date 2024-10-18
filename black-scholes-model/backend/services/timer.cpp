#include <Poco/DateTime.h>
#include <Poco/DateTimeParser.h>
#include <Poco/Timespan.h>
#include <iostream>
#include <string>

double calculateTime(const std::string& expirationDate) {
    // Parse the expiration date string in the format "YYYY-MM-DD"
    Poco::DateTime expirationDateTime;
    int utcOffset = 0; // Use an int for UTC offset
    
    // Parse the date
    Poco::DateTimeParser::parse(expirationDate, expirationDateTime, utcOffset);

    // Check if the parsed date is valid
    if (expirationDateTime.year() > 0) {
        // Get the current date using Poco
        Poco::DateTime now;

        // Calculate the timespan between now and the expiration date
        Poco::Timespan timeSpan = expirationDateTime - now;

        // Calculate the total days, converting to a double
        double totalDays = timeSpan.days() + 
                           timeSpan.hours() / 24.0 + 
                           timeSpan.minutes() / 1440.0 + 
                           timeSpan.seconds() / 86400.0;

        // Convert days to years
        double yearsToMaturity = totalDays / 365.0;

        return yearsToMaturity; // Return the time to maturity in years
    } else {
        std::cerr << "Error: Failed to parse the date." << std::endl;
        return -1.0; // Indicate error
    }
}