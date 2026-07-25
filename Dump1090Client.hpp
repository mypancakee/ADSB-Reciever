#pragma once

#include <string>
#include <stdexcept>
#include <thread>
#include <chrono>

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

    std::string host_address;
    int port_number;
    bool was_connected = false; // Tracks previous connection state to prevent log spamming

#ifdef _WIN32
    SOCKET socket_fd = INVALID_SOCKET;
#else
    int socket_fd = -1;
#endif


    // Helper method to safely clear resources
    void close_current_socket()
    {
#ifdef _WIN32
        if (socket_fd != INVALID_SOCKET)
        {
            closesocket(socket_fd);
            socket_fd = INVALID_SOCKET;
        }
#else
        if (socket_fd >= 0)
        {
            close(socket_fd);
            socket_fd = -1;
        }
#endif
    }


    // Reconnection routing subsystem loop
    void connect_to_server(bool is_initial_connection)
    {
        while (true)
        {
            try
            {
                close_current_socket();

                if (is_initial_connection)
                {
                    Debug::log("Dump1090Client", "Creating socket");
                }
                
                socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

#ifdef _WIN32
                if (socket_fd == INVALID_SOCKET)
#else
                if (socket_fd < 0)
#endif
                {
                    throw std::runtime_error("Failed to create socket");
                }

                sockaddr_in server_address{};
                server_address.sin_family = AF_INET;
                server_address.sin_port = htons(port_number);

                if (
                    inet_pton(
                        AF_INET,
                        host_address.c_str(),
                        &server_address.sin_addr
                    )
                    <= 0
                )
                {
                    throw std::runtime_error("Invalid IP address");
                }

                if (is_initial_connection)
                {
                    Debug::log(
                        "Dump1090Client",
                        "Connecting to " + host_address + ":" + std::to_string(port_number)
                    );
                }

                if (
                    connect(
                        socket_fd,
                        reinterpret_cast<sockaddr*>(&server_address),
                        sizeof(server_address)
                    )
                    < 0
                )
                {
                    throw std::runtime_error("Connection failed");
                }

                Debug::success("Dump1090Client", "Connected successfully to " + host_address + ":" + std::to_string(port_number));
                was_connected = true; // Connection established successfully
                return; 
            }
            catch (const std::runtime_error&)
            {
                close_current_socket();
                
                if (was_connected || is_initial_connection)
                {
                    // Print both lines (lost + trying) only on the initial crash event
                    Debug::error("Dump1090Client", "Connection to " + host_address + ":" + std::to_string(port_number) + " lost");
                    was_connected = false;
                }
                else
                {
                    // Subsequent failures only print the ongoing attempts log to avoid log spam
                    Debug::info("Dump1090Client", "Trying to connect to " + host_address + ":" + std::to_string(port_number));
                }
                
                std::this_thread::sleep_for(std::chrono::seconds(5));
                is_initial_connection = false; 
            }
        }
    }


public:

    Dump1090Client(
        const std::string& host,
        int port
    ) : host_address(host), port_number(port)
    {
#ifdef _WIN32
        Debug::log(
            "Dump1090Client",
            "Initializing Winsock"
        );

        WSADATA wsa_data{};
        int result = WSAStartup(MAKEWORD(2, 2), &wsa_data);

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

        // Kick off connection process
        connect_to_server(true);
    }


    ~Dump1090Client()
    {
        close_current_socket();

#ifdef _WIN32
        WSACleanup();
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
            int bytes_received = recv(socket_fd, &character, 1, 0);

            if (bytes_received <= 0)
            {
                // Disconnected mid-stream: let connect_to_server handle the single alert trigger
                connect_to_server(false);
                line.clear();
                continue;
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
