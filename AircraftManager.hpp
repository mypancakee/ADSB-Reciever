#pragma once

#include <unordered_map>
#include <string>
#include <utility>

#include "Aircraft.hpp"
#include "Debug.hpp"


class AircraftManager
{
private:

    std::unordered_map<
        std::string,
        Aircraft
    > aircraft;


public:

    // -----------------------------------------
    // Process a new message from dump1090
    // -----------------------------------------

    void process_message(
        const BaseStationMessage& message
    )
    {
        if (message.hex_ident.empty())
        {
            return;
        }


        auto [it, inserted] =
            aircraft.try_emplace(
                message.hex_ident,
                message.hex_ident
            );


        if (inserted)
        {
            Debug::success(
                "AircraftManager",
                "New aircraft added: "
                + message.hex_ident
            );
        }


        it->second.update(message);
    }


    // -----------------------------------------
    // Get one aircraft
    // -----------------------------------------

    Aircraft* get_aircraft(
        const std::string& hex_ident
    )
    {
        auto it =
            aircraft.find(hex_ident);


        if (it == aircraft.end())
        {
            return nullptr;
        }


        return &it->second;
    }


    // -----------------------------------------
    // Get all aircraft
    // -----------------------------------------

    const std::unordered_map<
        std::string,
        Aircraft
    >& get_all_aircraft() const
    {
        return aircraft;
    }


    // -----------------------------------------
    // Remove stale aircraft
    // -----------------------------------------

    void remove_stale_aircraft(
        int timeout_seconds
    )
    {
        for (
            auto it = aircraft.begin();
            it != aircraft.end();
        )
        {
            if (
                it->second.is_stale(
                    timeout_seconds
                )
            )
            {
                Debug::info(
                    "AircraftManager",
                    "Removing stale aircraft: "
                    + it->first
                );


                it =
                    aircraft.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }


    // -----------------------------------------
    // Number of aircraft
    // -----------------------------------------

    std::size_t aircraft_count() const
    {
        return aircraft.size();
    }
};