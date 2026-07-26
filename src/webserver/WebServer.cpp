#include "WebServer.hpp"

#include <iostream>
#include <string>
#include <stdexcept>

WebServer::WebServer(int port){
    serverSocket = socket(
        AF_INET,
        SOCK_STREAM,
        IPPROTO_TCP
    );

    if(serverSocket == INVALID_SOCKET){
        throw std::runtime_error("Failed to create TCP socket");
    }

    sockaddr_in address{};

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if(bind(
        serverSocket,
        reinterpret_cast<sockaddr*>(&address),
        sizeof(address)
    ) == SOCKET_ERROR){
        std::cerr << "bind() failed with error: "
                  << WSAGetLastError()
                  << '\n';

        closesocket(serverSocket);

        throw std::runtime_error("Failed to bind TCP socket");
    }

    if(listen(serverSocket, 10) == SOCKET_ERROR){
        std::cerr << "listen() failed with error: "
                  << WSAGetLastError()
                  << '\n';

        closesocket(serverSocket);

        throw std::runtime_error("Failed to listen on TCP socket");
    }

    std::cout << "WebServer listening on port "
              << port
              << '\n';
}

WebServer::~WebServer(){
    if(serverSocket != INVALID_SOCKET){
        closesocket(serverSocket);
    }
}

void WebServer::start(){
    std::cout << "WebServer started and waiting for connections...\n";

    while(true){
        SOCKET clientSocket = accept(
            serverSocket,
            nullptr,
            nullptr
        );

        if(clientSocket == INVALID_SOCKET){
            std::cerr << "accept() failed: "
                      << WSAGetLastError()
                      << '\n';

            throw std::runtime_error("Failed to accept client connection");
        }

        std::cout << "Client connected\n";

        std::string html =
            "<!DOCTYPE html>"
            "<html>"
            "<head>"
            "<meta charset=\"UTF-8\">"
            "<title>Aircraft Tracker</title>"
            "</head>"
            "<body>"
            "<h1>Aircraft Tracker</h1>"
            "<p>Web server is working!</p>"
            "</body>"
            "</html>";

        std::string response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html; charset=UTF-8\r\n"
            "Content-Length: " +
            std::to_string(html.size()) +
            "\r\n"
            "Connection: close\r\n"
            "\r\n" +
            html;

        int result = send(
            clientSocket,
            response.c_str(),
            static_cast<int>(response.size()),
            0
        );

        if(result == SOCKET_ERROR){
            std::cerr << "send() failed with error: "
                      << WSAGetLastError()
                      << '\n';
        }
        else{
            std::cout << "Sent "
                      << result
                      << " bytes\n";
        }

        closesocket(clientSocket);
    }
}