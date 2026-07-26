#pragma once

#include <string>

#include "nlohmann/json.hpp"
#include "Aircraft.hpp"


class AircraftJsonSerializer{
public:
    static std::string serialize(const Aircraft& aircraft){
        nlohmann::json json;

        json["hex_ident"] = aircraft.hex_ident;

        if(aircraft.callsign.has_value()){
            json["callsign"] = aircraft.callsign.value();
        }

        if(aircraft.latitude.has_value()){
            json["latitude"] = aircraft.latitude.value();
        }

        if(aircraft.longitude.has_value()){
            json["longitude"] = aircraft.longitude.value();
        }

        if(aircraft.altitude.has_value()){
            json["altitude"] = aircraft.altitude.value();
        }

        if(aircraft.ground_speed.has_value()){
            json["ground_speed"] = aircraft.ground_speed.value();
        }

        return json.dump();
    }
};