#include <iostream>

#include "./Debug.hpp"
#include "./Dump1090Client.hpp"
#include "./AircraftManager.hpp"
#include "./UdpBroadcaster.hpp"

int main()
{
    try
    {
        AircraftManager manager;

        Dump1090Client dump1090(
            "127.0.0.1",
            30003
        );

        UdpBroadcaster broadcaster(
            "192.168.2.255",
            4000
        );

        while (true)
        {
            std::string line =
                dump1090.read_line();

            // Send to everyone on the LAN
            broadcaster.send(line);

            // Optional
            BaseStationMessage message(line);

            manager.process_message(message);

            manager.remove_stale_aircraft(60);
        }
    }
    catch (const std::exception& e)
    {
        Debug::error(
            "Main",
            e.what()
        );
    }
}