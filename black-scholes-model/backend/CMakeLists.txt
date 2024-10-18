cmake_minimum_required(VERSION 3.14)
project(ScholesBackend)

# Set the C++ standard to C++20 (adjust as needed)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specify the build type (Debug/Release)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Find POCO
find_package(Poco REQUIRED COMPONENTS Foundation Net JSON Util NetSSL)

# Add the source and include directories
include_directories(include)

# Add executable
add_executable(ScholesBackend main.cpp model/BlackScholes.cpp)

# Link POCO libraries
target_link_libraries(ScholesBackend Poco::Foundation Poco::Net Poco::JSON Poco::Util Poco::NetSSL)

# Output binary into build/bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})