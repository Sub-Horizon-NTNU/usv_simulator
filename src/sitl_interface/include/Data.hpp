#pragma once

#include <nlohmann/json.hpp>
#include <cstdint>
#include <array>
#include <cstring>
#include <string>


struct sitl_data{
    uint16_t magic;
    uint16_t frame_rate;
    uint32_t frame_count;
    std::array<uint16_t,16> pwm;
};


struct simulator_data{
    double timestamp;
    std::array<double,3> gyro;
    std::array<double,3> accel_body;
    std::array<double,3> position;
    std::array<double,3> attitude;
    std::array<double,3> velocity;
};

void to_json(nlohmann::json &j, const simulator_data &sd);

struct gps_data {
    uint64_t time_usec{};         // (int) c_time/1e3,  
    uint8_t gps_id{};             // (uint8_t) ID of the GPS for multiple GPS inputs
    uint8_t fix_type{};           // (uint8_t) 0-1: no fix, 2: 2D fix, 3: 3D fix. 4: 3D with DGPS. 5: 3D with RTK
    int32_t lat{};                // (int32_t) Latitude (WGS84), in degrees * 1E7
    int32_t lon{};                // (int32_t) Longitude (WGS84), in degrees * 1E7
    float alt{};                  // (float) Altitude (AMSL, not WGS84), in m (positive for up)
    float hdop{};                 //   (float) GPS HDOP horizontal dilution of position in m
    float vdop{};                 //  (float) GPS VDOP vertical dilution of position in m
    float vn{};                   //  (float) GPS velocity in m/s in NORTH direction in earth-fixed NED frame
    float ve{};                   //  (float) GPS velocity in m/s in EAST direction in earth-fixed NED frame
    float vd{};                   //  (float) GPS velocity in m/s in DOWN direction in earth-fixed NED frame
    float speed_accuracy{};       // (float) GPS speed accuracy in m/s
    float horiz_accuracy{};       // (float) GPS horizontal accuracy in m
    float vert_accuracy{};        // (float) GPS vertical accuracy in m
    uint8_t satellites_visible{}; // (uint8_t) Number of satellites visible.
};

void to_json(nlohmann::json& j, const gps_data& g);

