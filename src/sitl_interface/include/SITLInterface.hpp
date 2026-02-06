#pragma once

#include <cmath>
#include <memory>
#include <array>
#include <nlohmann/json.hpp>
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/imu.hpp>
#include <sensor_msgs/msg/nav_sat_fix.hpp>
#include <sensor_msgs/msg/joint_state.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <std_msgs/msg/float64_multi_array.hpp>

#include <tf2/LinearMath/Quaternion.h>
#include <tf2/LinearMath/Matrix3x3.h>
#include <geometry_msgs/msg/quaternion.h>

#include "Data.hpp"
#include "UDPClient.hpp"

// Takes inspiration from Bluerobotics implementation for the bluerov: https://github.com/bvibhav/stonefish_bluerov2/blob/master/scripts/ardusim_patch.py (19.01.2026)
// 
class SITLInterface: public rclcpp::Node  
{
    public:

    SITLInterface();

    void imu_callback(const sensor_msgs::msg::Imu::SharedPtr msg);

    void odo_callback(const nav_msgs::msg::Odometry::SharedPtr msg);

    void gps_callback(const sensor_msgs::msg::NavSatFix::SharedPtr msg);


    void update();

    inline float scale(float x, float in_min, float in_max, float out_min, float out_max);

    private:
   
    nlohmann::json sim_data_j;
    nlohmann::json gps_data_j;
    std::array<double,3> rpy{0,0,0};
    simulator_data simulator_data_;
    gps_data gps_data_; 
    std::unique_ptr<UDPClient> udp_client_;
    
    rclcpp::Subscription<sensor_msgs::msg::Imu>::SharedPtr imu_sub_;
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odo_sub_;
    rclcpp::Subscription<sensor_msgs::msg::NavSatFix>::SharedPtr gps_sub_;

    rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr thruster_pub_;
    rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr servo_pub_;
    double c_time{};
    rclcpp::TimerBase::SharedPtr update_timer_;    
};
