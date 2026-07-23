#include <iostream>
#include <string>

#include "Debug.hpp"
#include "AircraftManager.hpp"
#include "Dump1090Client.hpp"


// --------------------------------------------------
// Clear the console screen
// --------------------------------------------------

void clear_screen()
{
    std::cout
        << "\033[2J\033[1;1H";
}


// --------------------------------------------------
// Print one aircraft
// --------------------------------------------------

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


// --------------------------------------------------
// Print all aircraft
// --------------------------------------------------

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


// --------------------------------------------------
// Main
// --------------------------------------------------

int main()
{
    Debug::log(
        "Main",
        "Program started"
    );


    try
    {
        Debug::log(
            "Main",
            "Creating AircraftManager"
        );


        AircraftManager manager;


        Debug::success(
            "Main",
            "AircraftManager created successfully"
        );


        Debug::log(
            "Main",
            "Connecting to dump1090"
        );


        Dump1090Client dump1090(
            "127.0.0.1",
            30003
        );


        Debug::success(
            "Main",
            "Dump1090 connection established"
        );


        Debug::log(
            "Main",
            "Entering main tracking loop"
        );


        while (true)
        {
            // --------------------------------------
            // Read one line from dump1090
            // --------------------------------------

            std::string line =
                dump1090.read_line();


            // --------------------------------------
            // Parse BaseStation message
            // --------------------------------------

            Debug::log(
                "Main",
                "Parsing BaseStation message"
            );


            BaseStationMessage message(
                line
            );


            // --------------------------------------
            // Process aircraft
            // --------------------------------------

            Debug::log(
                "Main",
                "Processing aircraft message"
            );


            manager.process_message(
                message
            );


            // --------------------------------------
            // Remove stale aircraft
            // --------------------------------------

            manager.remove_stale_aircraft(
                60
            );


            // --------------------------------------
            // Refresh display
            // --------------------------------------

            clear_screen();


            print_all_aircraft(
                manager
            );
        }
    }
    catch (
        const std::exception& exception
    )
    {
        Debug::error(
            "Main",
            exception.what()
        );


        return 1;
    }


    return 0;
}