#include <iostream>
#include <thread>
#include <nlohmann/json.hpp>

#include "WebServer.hpp"

#include "../Debug.hpp"
#include "./UdpReciever.hpp"


void receive_aircraft(){
    UdpReceiver receiver(4000);

    while(true){
        std::string line = receiver.read_line();

        nlohmann::json json =
            nlohmann::json::parse(line);

        std::cout << "ICAO: "
                  << json["hex_ident"]
                  << '\n';
    }
}


int main(){
    WSADATA wsaData{};

    if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0){
        std::cerr << "WSAStartup failed\n";

        return 1;
    }

    try{
        WebServer webServer(8080);

        std::thread aircraftThread(receive_aircraft);

        webServer.start();

        aircraftThread.join();
    }
    catch(const std::exception& e){
        Debug::error("Main", e.what());
    }

    WSACleanup();

    return 0;
}