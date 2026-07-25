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

class UdpReceiver
{
private:

#ifdef _WIN32

    SOCKET socket_fd = INVALID_SOCKET;

#else

    int socket_fd = -1;

#endif

public:

    explicit UdpReceiver(
        int port
    )
    {

#ifdef _WIN32

        WSADATA wsa_data{};

        if (
            WSAStartup(
                MAKEWORD(2, 2),
                &wsa_data
            ) != 0
        )
        {
            throw std::runtime_error(
                "WSAStartup failed"
            );
        }

#endif

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

        sockaddr_in address{};

        address.sin_family =
            AF_INET;

        address.sin_port =
            htons(port);

        address.sin_addr.s_addr =
            INADDR_ANY;

        if (
            bind(
                socket_fd,
                reinterpret_cast<sockaddr*>(
                    &address
                ),
                sizeof(address)
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
                "Failed to bind UDP socket"
            );
        }

        Debug::success(
            "UdpReceiver",
            "Listening on UDP port "
            + std::to_string(port)
        );
    }

    ~UdpReceiver()
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
            "UdpReceiver",
            "Socket closed"
        );
    }

    std::string read_line()
    {
        char buffer[2048];

        int bytes_received =
            recvfrom(
                socket_fd,
                buffer,
                sizeof(buffer) - 1,
                0,
                nullptr,
                nullptr
            );

        if (bytes_received <= 0)
        {
            throw std::runtime_error(
                "Failed to receive UDP packet"
            );
        }

        buffer[bytes_received] = '\0';

        return std::string(buffer);
    }
};