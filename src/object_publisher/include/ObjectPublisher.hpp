


#pragma once

#include "rclcpp/rclcpp.hpp"

#include "object_msgs/msg/buoy.hpp"
#include "object_msgs/msg/buoys.hpp"
#include "object_msgs/msg/boat.hpp"
#include "object_msgs/msg/boats.hpp"

#include <nav_msgs/msg/odometry.hpp>
#include <string>
#include <vector>
#include <cmath>
#include <functional>
#include <tf2/LinearMath/Matrix3x3.h>

#include <tf2/LinearMath/Quaternion.h>
#include <chrono>
#include <pugixml.hpp>


class ObjectPublisher : public rclcpp::Node
{
public:
    ObjectPublisher() : Node("object_publisher")
    {   
        // Simulator origin NED reference
        start_position_.pose.pose.position.x = 0;
        start_position_.pose.pose.position.y = 0;
        start_position_.pose.pose.position.z = 0;
    
        
        add_buoys(10,5,0,"red",0,10.0f,10);
        add_buoys(10,10,0,"green",0,10.0f,10);


        position_subscriber_= this->create_subscription<nav_msgs::msg::Odometry>(
            "/mavros/global_position/local", 10, std::bind(&ObjectPublisher::position_cb, this, std::placeholders::_1));
        
        buoy_publisher_ = this->create_publisher<object_msgs::msg::Buoys>("/simulator/buoys",10);
        boat_publisher_ = this->create_publisher<object_msgs::msg::Boats>("/simulator/boats",10);
       
        buoy_timer_ = this->create_wall_timer(
            std::chrono::milliseconds(100),
            std::bind(&ObjectPublisher::publish_buoys, this));
        }

    void parse_buoys(){
        //pugi::xml_document doc;


    }
    
    void position_cb(nav_msgs::msg::Odometry position_msg){
        current_position_.pose.pose.position.x = position_msg.pose.pose.position.x;
        current_position_.pose.pose.position.y = position_msg.pose.pose.position.y;
        current_position_.pose.pose.position.z = position_msg.pose.pose.position.z;
     
        tf2::Quaternion q(
            position_msg.pose.pose.orientation.x,
            position_msg.pose.pose.orientation.y,
            position_msg.pose.pose.orientation.z,
            position_msg.pose.pose.orientation.w
        );

        tf2::Matrix3x3 m(q);
        double roll, pitch, yaw;
        m.getRPY(roll, pitch, yaw);
        current_heading_ = yaw;
    }

    

    void publish_buoys(){
        object_msgs::msg::Buoys buoy_pub_msg;
        buoy_pub_msg = buoys_msg;

        double position_x =current_position_.pose.pose.position.x;
        double position_y =current_position_.pose.pose.position.y;

        for(auto buoy: buoy_pub_msg){
            if((relative_angle(position_x,position_y,buoy.x,buoy.y)<=field_of_view_deg_*(M_PI/180)) && (std::hypot(position_x-buoy.x,position_y-buoy.y)) <= radius_){
                RCLCPP_INFO(this->get_logger(), "Buoy detected at x: %f y:%f",buoy.x,buoy.y);

            }
        }
    }

    inline double relative_angle(const double &usv_x, const double &usv_y, const double &buoy_x, const double &buoy_y){
        return angle = std::atan2(usv_x-buoy_x,usv_y-buoy_y)
    }

    void add_buoys(double start_x, double start_y, double z, std::string color, double heading, double space, int amount){
        double spacing_x = space*std::cos(heading);
        double spacing_y = space*std::sin(heading);
        for(int i = 0; i < amount; i++){
            add_buoy(start_x+spacing_x*static_cast<double>(i), start_y+spacing_y*static_cast<double>(i), z, color);
        }
    }

    void add_buoy(double x, double y, double z, std::string color){
        object_msgs::msg::Buoy buoy;
        buoy.pos_x = x; buoy.pos_y = y; buoy.pos_z = z;
        buoy.color = color;
        buoys_msg.buoys.push_back(buoy);
        buoys_msg.amount +=1;
    }

private:
    nav_msgs::msg::Odometry start_position_;
    double current_heading_{};
    nav_msgs::msg::Odometry current_position_;
    object_msgs::msg::Buoys buoys_;
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr position_subscriber_;
    rclcpp::Publisher<object_msgs::msg::Buoys>::SharedPtr buoy_publisher_;
    rclcpp::Publisher<object_msgs::msg::Boats>::SharedPtr boat_publisher_;

    rclcpp::TimerBase::SharedPtr buoy_timer_;
    uint32_t buoy_count_{};
    const double radius_;
    const double field_of_view_deg_=78.0;
    



};
