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


class UdpBroadcaster
{
private:

#ifdef _WIN32

    SOCKET socket_fd = INVALID_SOCKET;

#else

    int socket_fd = -1;

#endif


    sockaddr_in destination{};


public:

    UdpBroadcaster(
        const std::string& broadcast_ip,
        int port
    )
    {

#ifdef _WIN32

        Debug::log(
            "UdpBroadcaster",
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
            "UdpBroadcaster",
            "Winsock initialized"
        );

#endif


        Debug::log(
            "UdpBroadcaster",
            "Creating UDP socket"
        );


        socket_fd =
            socket(
                AF_INET,
                SOCK_DGRAM,
                IPPROTO_UDP
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
                "Failed to create UDP socket"
            );
        }


        // Enable LAN broadcast

        int enable = 1;


        if (
            setsockopt(
                socket_fd,
                SOL_SOCKET,
                SO_BROADCAST,

#ifdef _WIN32

                reinterpret_cast<const char*>(
                    &enable
                ),

#else

                &enable,

#endif

                sizeof(enable)
            )
#ifdef _WIN32
            == SOCKET_ERROR
#else
            < 0
#endif
        )
        {

#ifdef _WIN32

            closesocket(socket_fd);
            WSACleanup();

#else

            close(socket_fd);

#endif

            throw std::runtime_error(
                "Failed to enable broadcast"
            );
        }


        destination.sin_family =
            AF_INET;


        destination.sin_port =
            htons(port);


        if (
            inet_pton(
                AF_INET,
                broadcast_ip.c_str(),
                &destination.sin_addr
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
                "Invalid broadcast IP address"
            );
        }


        Debug::success(
            "UdpBroadcaster",
            "Broadcasting on "
            + broadcast_ip
            + ":"
            + std::to_string(port)
        );
    }



    ~UdpBroadcaster()
    {

#ifdef _WIN32

        if (
            socket_fd != INVALID_SOCKET
        )
        {
            closesocket(socket_fd);
        }


        WSACleanup();


#else

        if (
            socket_fd >= 0
        )
        {
            close(socket_fd);
        }

#endif


        Debug::log(
            "UdpBroadcaster",
            "Socket closed"
        );
    }



    void send(
        const std::string& message
    )
    {

        int result =
            sendto(
                socket_fd,
                message.c_str(),
                static_cast<int>(
                    message.size()
                ),
                0,
                reinterpret_cast<sockaddr*>(
                    &destination
                ),
                sizeof(destination)
            );


#ifdef _WIN32

        if (result == SOCKET_ERROR)

#else

        if (result < 0)

#endif

        {
            throw std::runtime_error(
                "Failed to send UDP broadcast"
            );
        }
    }
};