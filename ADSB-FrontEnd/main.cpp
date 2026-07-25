#include <iostream>
#include <string>

#include "./Debug.hpp"
#include "./AircraftManager.hpp"
#include "./BaseStationMessage.hpp"
#include "UdpReciever.hpp"

// Your clear_screen(), print_aircraft(),
// and print_all_aircraft() stay exactly the same.

void clear_screen()
{
    std::cout
        << "\033[2J\033[1;1H";
}

void print_aircraft(
    const Aircraft& aircraft)
{
    std::cout
        << "-----------------------------\n";


    std::cout
        << "ICAO: "
        << aircraft.hex_ident
        << '\n';


    std::cout
        << "Callsign: ";

    if (
        aircraft.callsign.has_value()
    )
    {
        std::cout
            << aircraft.callsign.value();
    }
    else
    {
        std::cout
            << "Unknown";
    }

    std::cout
        << '\n';


    std::cout
        << "Altitude: ";

    if (
        aircraft.altitude.has_value()
    )
    {
        std::cout
            << aircraft.altitude.value()
            << " ft";
    }
    else
    {
        std::cout
            << "Unknown";
    }

    std::cout
        << '\n';


    std::cout
        << "Position: ";

    if (
        aircraft.latitude.has_value()
        &&
        aircraft.longitude.has_value()
    )
    {
        std::cout
            << aircraft.latitude.value()
            << ", "
            << aircraft.longitude.value();
    }
    else
    {
        std::cout
            << "Unknown";
    }

    std::cout
        << '\n';


    std::cout
        << "Ground Speed: ";

    if (
        aircraft.ground_speed.has_value()
    )
    {
        std::cout
            << aircraft.ground_speed.value()
            << " knots";
    }
    else
    {
        std::cout
            << "Unknown";
    }

    std::cout
        << '\n';


    std::cout
        << "Track: ";

    if (
        aircraft.track.has_value()
    )
    {
        std::cout
            << aircraft.track.value()
            << " degrees";
    }
    else
    {
        std::cout
            << "Unknown";
    }

    std::cout
        << '\n';


    std::cout
        << "Last Seen: "
        << aircraft
            .seconds_since_last_seen()
            .count()
        << " seconds ago\n";
}


void print_all_aircraft(
    const AircraftManager& manager)
{
    std::cout
        << "=================================\n";

    std::cout
        << "       CURRENT AIRCRAFT\n";

    std::cout
        << "=================================\n";


    std::cout
        << "Aircraft tracked: "
        << manager.aircraft_count()
        << "\n\n";


    for (
        const auto& [hex_ident, aircraft]
        :
        manager.get_all_aircraft()
    )
    {
        print_aircraft(
            aircraft
        );
    }
}


int main()
{
    Debug::log(
        "Main",
        "Program started"
    );

    try
    {
        AircraftManager manager;

        Debug::log(
            "Main",
            "Starting UDP receiver"
        );

        UdpReceiver receiver(
            4000
        );

        Debug::success(
            "Main",
            "Listening for aircraft data"
        );

        while (true)
        {
            std::string line =
                receiver.read_line();

            BaseStationMessage message(
                line
            );

            manager.process_message(
                message
            );

            manager.remove_stale_aircraft(
                60
            );

            clear_screen();

            print_all_aircraft(
                manager
            );
        }
    }
    catch (const std::exception& exception)
    {
        Debug::error(
            "Main",
            exception.what()
        );

        return 1;
    }

    return 0;
}