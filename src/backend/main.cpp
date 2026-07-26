#include <iostream>

#include <nlohmann/json.hpp>
#include "../Debug.hpp"
#include "../Dump1090Client.hpp"
#include "../AircraftManager.hpp"
#include "./UdpBroadcaster.hpp"


int main(){
    try{
        AircraftManager manager;

        Dump1090Client dump1090("127.0.0.1", 30003);

        UdpBroadcaster broadcaster("192.168.2.255", 4000);

        while(true){
            std::string line = dump1090.read_line();

            BaseStationMessage message(line);

            manager.process_message(message);

            Aircraft* aircraft = manager.get_aircraft(message.hex_ident);

            if(aircraft != nullptr){
                nlohmann::json json;

                json["hex_ident"] = aircraft->hex_ident;

                if(aircraft->callsign.has_value()){
                    json["callsign"] = aircraft->callsign.value();
                }

                if(aircraft->flight_id.has_value()){
                    json["flight_id"] = aircraft->flight_id.value();
                }

                if(aircraft->latitude.has_value()){
                    json["latitude"] = aircraft->latitude.value();
                }

                if(aircraft->longitude.has_value()){
                    json["longitude"] = aircraft->longitude.value();
                }

                if(aircraft->altitude.has_value()){
                    json["altitude"] = aircraft->altitude.value();
                }

                if(aircraft->ground_speed.has_value()){
                    json["ground_speed"] = aircraft->ground_speed.value();
                }

                if(aircraft->track.has_value()){
                    json["track"] = aircraft->track.value();
                }

                if(aircraft->vertical_rate.has_value()){
                    json["vertical_rate"] = aircraft->vertical_rate.value();
                }

                if(aircraft->squawk.has_value()){
                    json["squawk"] = aircraft->squawk.value();
                }

                if(aircraft->alert.has_value()){
                    json["alert"] = aircraft->alert.value();
                }

                if(aircraft->emergency.has_value()){
                    json["emergency"] = aircraft->emergency.value();
                }

                if(aircraft->spi.has_value()){
                    json["spi"] = aircraft->spi.value();
                }

                if(aircraft->is_on_ground.has_value()){
                    json["is_on_ground"] = aircraft->is_on_ground.value();
                }

                json["last_seen_seconds"] = aircraft->seconds_since_last_seen().count();

                broadcaster.send(json.dump());
            }

            manager.remove_stale_aircraft(60);
        }
    }
    catch(const std::exception& e){
        Debug::error("Main", e.what());
    }
}