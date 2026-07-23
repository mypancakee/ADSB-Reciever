#pragma once

#include <iostream>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>


class Debug
{
private:

    // ANSI color codes

    static constexpr const char* RESET =
        "\033[0m";

    static constexpr const char* YELLOW =
        "\033[33m";

    static constexpr const char* GREEN =
        "\033[32m";

    static constexpr const char* RED =
        "\033[31m";

    static constexpr const char* CYAN =
        "\033[36m";


    // -----------------------------------------
    // Get current time as HH:MM:SS
    // -----------------------------------------

    static std::string current_time()
    {
        auto now =
            std::chrono::system_clock::now();


        std::time_t current_time =
            std::chrono::system_clock::to_time_t(
                now
            );


        std::tm local_time{};


#ifdef _WIN32

        localtime_s(
            &local_time,
            &current_time
        );

#else

        localtime_r(
            &current_time,
            &local_time
        );

#endif


        std::ostringstream time_stream;


        time_stream
            << std::put_time(
                &local_time,
                "%H:%M:%S"
            );


        return time_stream.str();
    }


public:

    static void log(
        const std::string& class_name,
        const std::string& message
    )
    {
        std::cout
            << YELLOW
            << "["
            << current_time()
            << "] "
            << "[DEBUG] "
            << class_name
            << ": "
            << message
            << RESET
            << '\n';
    }


    static void success(
        const std::string& class_name,
        const std::string& message
    )
    {
        std::cout
            << GREEN
            << "["
            << current_time()
            << "] "
            << "[SUCCESS] "
            << class_name
            << ": "
            << message
            << RESET
            << '\n';
    }


    static void error(
        const std::string& class_name,
        const std::string& message
    )
    {
        std::cerr
            << RED
            << "["
            << current_time()
            << "] "
            << "[ERROR] "
            << class_name
            << ": "
            << message
            << RESET
            << '\n';
    }


    static void info(
        const std::string& class_name,
        const std::string& message
    )
    {
        std::cout
            << CYAN
            << "["
            << current_time()
            << "] "
            << "[INFO] "
            << class_name
            << ": "
            << message
            << RESET
            << '\n';
    }
};