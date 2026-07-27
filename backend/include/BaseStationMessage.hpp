#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <optional>

class BaseStationMessage {
public:
    // Required BaseStation fields
    std::string message_type;
    int transmission_type = 0;
    std::string session_id;
    std::string aircraft_id;
    std::string hex_ident;
    std::optional<std::string> flight_id;
    std::string date_generated;
    std::string time_generated;
    std::string date_logged;
    std::string time_logged;

    // Optional dynamic fields
    std::optional<std::string> callsign;
    std::optional<int> altitude;
    std::optional<double> ground_speed;
    std::optional<double> track;
    std::optional<double> latitude;
    std::optional<double> longitude;
    std::optional<int> vertical_rate;
    std::optional<std::string> squawk;
    std::optional<bool> alert;
    std::optional<bool> emergency;
    std::optional<bool> spi;
    std::optional<bool> is_on_ground;

    // Constructor
    explicit BaseStationMessage(const std::string& csv_line) {
        parse_csv(csv_line);
    }

private:

    template <typename NumType>
    std::optional<NumType> to_numeric(const std::string& str) {
        if (str.empty()) {
            return std::nullopt;
        }

        std::stringstream ss(str);
        NumType value;

        if (ss >> value) {
            return value;
        }

        return std::nullopt;
    }

    std::optional<bool> to_bool(const std::string& str) {
        if (str.empty()) {
            return std::nullopt;
        }

        if (str == "1" || str == "-1") {
            return true;
        }

        if (str == "0") {
            return false;
        }

        return std::nullopt;
    }

    void parse_csv(const std::string& line) {
        std::vector<std::string> tokens;
        std::stringstream ss(line);
        std::string token;

        while (std::getline(ss, token, ',')) {
            tokens.push_back(token);
        }

        if (tokens.size() < 22) {
            return;
        }

        // Required fields
        message_type      = tokens[0];
        transmission_type = to_numeric<int>(tokens[1]).value_or(0);
        session_id        = tokens[2];
        aircraft_id       = tokens[3];
        hex_ident         = tokens[4];
        flight_id         = tokens[5];
        date_generated    = tokens[6];
        time_generated    = tokens[7];
        date_logged       = tokens[8];
        time_logged       = tokens[9];

        // Optional fields
        if (!tokens[10].empty()) {
            callsign = tokens[10];
        }

        altitude      = to_numeric<int>(tokens[11]);
        ground_speed  = to_numeric<double>(tokens[12]);
        track         = to_numeric<double>(tokens[13]);
        latitude      = to_numeric<double>(tokens[14]);
        longitude     = to_numeric<double>(tokens[15]);
        vertical_rate = to_numeric<int>(tokens[16]);

        if (!tokens[17].empty()) {
            squawk = tokens[17];
        }

        alert        = to_bool(tokens[18]);
        emergency    = to_bool(tokens[19]);
        spi          = to_bool(tokens[20]);
        is_on_ground = to_bool(tokens[21]);
    }
};