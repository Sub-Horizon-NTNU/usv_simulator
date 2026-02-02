#include "Data.hpp"


void to_json(nlohmann::json &j, const simulator_data &sd){
    j = nlohmann::json{
        {"timestamp",sd.timestamp},
       {"imu", {
            {"gyro", sd.gyro},
            {"accel_body", sd.accel_body}
        }},
        {"position",sd.position},
        {"attitude",sd.attitude},
        {"velocity",sd.velocity},
    };
}



void to_json(nlohmann::json& j, const gps_data& g) {
    j = nlohmann::json{
        {"time_usec", g.time_usec},
        {"gps_id", g.gps_id},
        {"fix_type", g.fix_type},
        {"lat", g.lat},
        {"lon", g.lon},
        {"alt", g.alt},
        {"hdop", g.hdop},
        {"vdop", g.vdop},
        {"vn", g.vn},
        {"ve", g.ve},
        {"vd", g.vd},
        {"speed_accuracy", g.speed_accuracy},
        {"horiz_accuracy", g.horiz_accuracy},
        {"vert_accuracy", g.vert_accuracy},
        {"satellites_visible", g.satellites_visible}
    };
}
