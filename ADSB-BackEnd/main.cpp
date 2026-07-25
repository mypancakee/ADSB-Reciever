#include <iostream>

#include "../Debug.hpp"
#include "../Dump1090Client.hpp"
#include "../AircraftManager.hpp"
#include "UdpBroadcaster.hpp"

int main(){

    Dump1090Client dump1090("127.0.0.1, 30003");
    AircraftManager manager;
    UdpBroadcaster broadcaster("192.168.2.255",4000);

    while (true)
    {
        std::string line =
            dump1090.read_line();

        BaseStationMessage message(line);

        manager.process_message(message);

        Aircraft* aircraft =
            manager.get_aircraft(
                message.hex_ident
            );

        if (aircraft != nullptr)
        {
            broadcaster.send(
                serialize_aircraft(*aircraft)
            );
        }

        manager.remove_stale_aircraft(60);
    }
}