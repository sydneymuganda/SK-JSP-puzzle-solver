cmake_minimum_required(VERSION 3.14)
project(RedisDB LANGUAGES C CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Specify the build type (Debug/Release)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Add the include directories
include_directories(include)

# Add executable and source files
set(SOURCES
    main.cpp
    service/server.cpp
    service/client.cpp
    # service/helper.c  
)

# Add executable and link C and C++ sources
add_executable(RedisDB ${SOURCES})

# Output binary into build/bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})