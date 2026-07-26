#pragma once

#include <string>
#include <optional>
#include <chrono>
#include <utility> // For std::move
#include <deque>

#include "BaseStationMessage.hpp"

struct AircraftTrackPoint
{
    double latitude;
    double longitude;

    int altitude;

    double ground_speed;

    std::chrono::steady_clock::time_point timestamp;
};

class Aircraft {
private:
    // 1. For matching std::optional types (altitude, latitude, etc.)
    template <typename T>
    static void update_if_present(std::optional<T>& target, const std::optional<T>& source) {
        if (source.has_value()) {
            target = source;
        }
    }

    // 2. For plain string to plain string (hex_ident, timestamps)
    static void update_if_not_empty(std::string& target, const std::string& source) {
        if (!source.empty()) {
            target = source;
        }
    }

    // 3. NEW: For plain string source to std::optional target (flight_id)
    static void update_if_not_empty(std::optional<std::string>& target, const std::string& source) {
        if (!source.empty()) {
            target = source;
        }
    }
public:
    // Identity
    std::string hex_ident;
    std::optional<std::string> callsign;
    std::optional<std::string> flight_id;

    // Position
    std::optional<double> latitude;
    std::optional<double> longitude;

    // Movement
    std::optional<int> altitude;
    std::optional<double> ground_speed;
    std::optional<double> track;
    std::optional<int> vertical_rate;

    // Transponder information
    std::optional<std::string> squawk;

    // Status
    std::optional<bool> alert;
    std::optional<bool> emergency;
    std::optional<bool> spi;
    std::optional<bool> is_on_ground;

    // Metadata
    std::string date_generated;
    std::string time_generated;
    std::string date_logged;
    std::string time_logged;

    std::chrono::steady_clock::time_point last_seen;

    // Historic Data
    std::deque<AircraftTrackPoint> track_history;

public:
    Aircraft() : last_seen(std::chrono::steady_clock::now()) {}

    explicit Aircraft(const std::string& hex_ident)
        : hex_ident(hex_ident), last_seen(std::chrono::steady_clock::now()) {}

    // Cleaned up update method
    void update(const BaseStationMessage& message) {
        last_seen = std::chrono::steady_clock::now();

        // Identity & Metadata (Strings)
        update_if_not_empty(hex_ident,      message.hex_ident);
        update_if_not_empty(date_generated, message.date_generated);
        update_if_not_empty(time_generated, message.time_generated);
        update_if_not_empty(date_logged,    message.date_logged);
        update_if_not_empty(time_logged,    message.time_logged);

        // Optionals (Using the template helper)
        update_if_present(callsign,       message.callsign);
        update_if_present(flight_id,      message.flight_id);
        update_if_present(latitude,       message.latitude);
        update_if_present(longitude,      message.longitude);
        update_if_present(altitude,       message.altitude);
        update_if_present(ground_speed,   message.ground_speed);
        update_if_present(track,          message.track);
        update_if_present(vertical_rate,  message.vertical_rate);
        update_if_present(squawk,         message.squawk);
        update_if_present(alert,          message.alert);
        update_if_present(emergency,      message.emergency);
        update_if_present(spi,            message.spi);
        update_if_present(is_on_ground,   message.is_on_ground);

        if (message.latitude.has_value() && message.longitude.has_value() && message.altitude.has_value() && message.ground_speed.has_value()){
            track_history.push_back(
                {
                    message.latitude.value(),
                    message.longitude.value(),
                    message.altitude.value(),
                    message.ground_speed.value(),
                    last_seen
                }
            );
        }
    }

    std::chrono::seconds seconds_since_last_seen() const {
        return std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now() - last_seen
        );
    }

    bool is_stale(int timeout_seconds = 60) const {
        return seconds_since_last_seen().count() >= timeout_seconds;
    }

};