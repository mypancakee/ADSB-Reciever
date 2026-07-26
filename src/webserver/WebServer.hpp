#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>

#include <stdexcept>

class WebServer{
private:
    SOCKET serverSocket = INVALID_SOCKET;

public:
    explicit WebServer(int port);

    ~WebServer();

    void start();
};