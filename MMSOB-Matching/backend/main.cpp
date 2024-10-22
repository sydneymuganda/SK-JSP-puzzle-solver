#include <iostream>
#include <thread>
#include "./controller/Server.cpp" 

int main(int argc, char** argv) {
   
    std::thread serverThread(runServer);
    serverThread.detach(); 

    
    std::cout << "Main application running... Press CTRL+C to exit." << std::endl;

    
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0; 
}