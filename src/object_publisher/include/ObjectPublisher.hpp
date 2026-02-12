


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

        //Parameters for detection system
        this->declare_parameter<double>("max_detection_radius", 20.0);
        this->declare_parameter<double>("min_detection_radius", 0.6);
        this->declare_parameter<double>("field_of_view", 72.0);
        this->declare_parameter<int>("detection_rate", 100);
        
        max_detection_radius_ = this->get_parameter("max_detection_radius").get_value<double>();
        max_detection_radius_ = this->get_parameter("min_detection_radius").get_value<double>();
        max_detection_radius_ = this->get_parameter("field_of_view").get_value<double>();
        int detection_rate = this->get_parameter("detection_rate").get_value<int>();
        
        //Add buoys to the map ("Hardcoded", must be same in the simulator)
        add_buoys(10,5,0,"Red",0,10.0f,10);
        add_buoys(10,10,0,"Green",0,10.0f,10);

        auto qos_geo_pose = rclcpp::QoS(rclcpp::KeepLast(1))
        .reliability(rclcpp::ReliabilityPolicy::BestEffort)
        .durability_volatile();

        position_subscriber_= this->create_subscription<nav_msgs::msg::Odometry>(
            "/sensors/odometry", qos_geo_pose, std::bind(&ObjectPublisher::position_cb, this, std::placeholders::_1));
        
        buoy_publisher_ = this->create_publisher<object_msgs::msg::Buoys>("/simulator/buoys",10);
        boat_publisher_ = this->create_publisher<object_msgs::msg::Boats>("/simulator/boats",10);
       
        buoy_timer_ = this->create_wall_timer(
            std::chrono::milliseconds(detection_rate),
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

        double position_x =current_position_.pose.pose.position.x;
        double position_y =current_position_.pose.pose.position.y;

        // run throught all buoys and check angle and distance
        for(const auto &buoy : buoys_.buoys){ 
            double distance = std::hypot(buoy.pos_x-position_x,buoy.pos_y-position_y);
            double buoy_angle = std::atan2(buoy.pos_y-position_y,buoy.pos_x-position_x);
            double angle_diff = buoy_angle-current_heading_;
            
            if((distance < max_detection_radius_) && (distance > min_detection_radius_) && (std::abs(angle_diff)<= field_of_view_deg_*0.5*M_PI/180)){
                //RCLCPP_DEBUG(this->get_logger(), "%s buoy detected at x_diff: %f y_diff:%f | buoy_pos: x: %f y %f | boat_pos: x: position_x: %f y: position_y: %f | dist: %f, angle: %f, buoy angle: %f",
                //buoy.color.c_str(), position_x-buoy.pos_x, position_y-buoy.pos_y, buoy.pos_x, buoy.pos_y, position_x, position_y,distance,angle_diff*180/M_PI,buoy_angle*180/M_PI);
                object_msgs::msg::Buoy buoy;
                buoy.pos_x = std::cos(angle_diff)*distance;
                buoy.pos_y = std::sin(angle_diff)*distance;
                buoy.pos_z = distance;
                buoy_pub_msg.amount += 1;
                buoy_pub_msg.buoys.push_back(buoy);
                RCLCPP_INFO(this->get_logger(), "rel_x: %f, rel_y: %f, distance: %f",buoy.pos_x,buoy.pos_y,distance);
            }
        }
        if(buoy_pub_msg.amount >0){
            buoy_publisher_->publish(buoy_pub_msg);
        }
    }
    //For creating multiple buoys in a line with a given spacing and angle (0 deg is North).
    void add_buoys(double start_x, double start_y, double z, std::string color, double heading, double space, int amount){
        double spacing_x = space*std::cos(heading);
        double spacing_y = space*std::sin(heading);
        for(int i = 0; i < amount; i++){
            add_buoy(start_x+spacing_x*static_cast<double>(i), start_y+spacing_y*static_cast<double>(i), z, color);
        }
    }

    //create a single buoy and add it to the list of buoys.
    void add_buoy(double x, double y, double z, std::string color){
        object_msgs::msg::Buoy buoy;

        buoy.pos_x = x; buoy.pos_y = y; buoy.pos_z = z;
        buoy.color = color;

        buoys_.buoys.push_back(buoy);
        buoys_.amount +=1;
    }

private:
    double max_detection_radius_ = 20.0;
    double min_detection_radius_ = 0.6;
    double field_of_view_deg_=72.0;

    
    nav_msgs::msg::Odometry start_position_;
    nav_msgs::msg::Odometry current_position_;
    double current_heading_{};
    
    object_msgs::msg::Buoys buoys_;
    object_msgs::msg::Boats boats_;
    
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr position_subscriber_;
    rclcpp::Publisher<object_msgs::msg::Buoys>::SharedPtr buoy_publisher_;
    rclcpp::Publisher<object_msgs::msg::Boats>::SharedPtr boat_publisher_;
    rclcpp::TimerBase::SharedPtr buoy_timer_;
    



};
