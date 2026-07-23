#pragma once

#include <string>
#include <stdexcept>

#ifdef _WIN32

#include <winsock2.h>
#include <ws2tcpip.h>

#else

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#endif

#include "Debug.hpp"


class Dump1090Client
{
private:

#ifdef _WIN32

    SOCKET socket_fd = INVALID_SOCKET;

#else

    int socket_fd = -1;

#endif


public:

    Dump1090Client(
        const std::string& host,
        int port
    )
    {

#ifdef _WIN32

        Debug::log(
            "Dump1090Client",
            "Initializing Winsock"
        );


        WSADATA wsa_data{};


        int result =
            WSAStartup(
                MAKEWORD(2, 2),
                &wsa_data
            );


        if (result != 0)
        {
            throw std::runtime_error(
                "WSAStartup failed"
            );
        }


        Debug::success(
            "Dump1090Client",
            "Winsock initialized successfully"
        );

#endif


        Debug::log(
            "Dump1090Client",
            "Creating socket"
        );


        socket_fd =
            socket(
                AF_INET,
                SOCK_STREAM,
                IPPROTO_TCP
            );


#ifdef _WIN32

        if (socket_fd == INVALID_SOCKET)

#else

        if (socket_fd < 0)

#endif

        {

#ifdef _WIN32

            WSACleanup();

#endif

            throw std::runtime_error(
                "Failed to create socket"
            );
        }


        Debug::success(
            "Dump1090Client",
            "Socket created successfully"
        );


        sockaddr_in server_address{};


        server_address.sin_family =
            AF_INET;


        server_address.sin_port =
            htons(port);


        if (
            inet_pton(
                AF_INET,
                host.c_str(),
                &server_address.sin_addr
            )
            <= 0
        )
        {

#ifdef _WIN32

            closesocket(socket_fd);

            WSACleanup();

#else

            close(socket_fd);

#endif

            throw std::runtime_error(
                "Invalid IP address"
            );
        }


        Debug::log(
            "Dump1090Client",
            "Connecting to "
            + host
            + ":"
            + std::to_string(port)
        );


        if (
            connect(
                socket_fd,
                reinterpret_cast<sockaddr*>(
                    &server_address
                ),
                sizeof(server_address)
            )
            < 0
        )
        {

#ifdef _WIN32

            closesocket(socket_fd);

            WSACleanup();

#else

            close(socket_fd);

#endif

            throw std::runtime_error(
                "Failed to connect to dump1090"
            );
        }


        Debug::success(
            "Dump1090Client",
            "Connected successfully to dump1090"
        );
    }


    ~Dump1090Client()
    {

#ifdef _WIN32

        if (socket_fd != INVALID_SOCKET)
        {
            closesocket(socket_fd);
        }

        WSACleanup();

#else

        if (socket_fd >= 0)
        {
            close(socket_fd);
        }

#endif

        Debug::log(
            "Dump1090Client",
            "Connection closed"
        );
    }


    std::string read_line()
    {
        std::string line;

        char character;


        while (true)
        {

#ifdef _WIN32

            int bytes_received =
                recv(
                    socket_fd,
                    &character,
                    1,
                    0
                );

#else

            int bytes_received =
                recv(
                    socket_fd,
                    &character,
                    1,
                    0
                );

#endif


            if (bytes_received <= 0)
            {
                throw std::runtime_error(
                    "Connection to dump1090 lost"
                );
            }


            if (character == '\n')
            {
                break;
            }


            if (character != '\r')
            {
                line += character;
            }
        }


        return line;
    }
};