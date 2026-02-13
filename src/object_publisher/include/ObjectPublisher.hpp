


#pragma once

#include "rclcpp/rclcpp.hpp"

#include "object_msgs/msg/buoy.hpp"
#include "object_msgs/msg/buoys.hpp"
#include "object_msgs/msg/boat.hpp"
#include "object_msgs/msg/boats.hpp"

#include <nav_msgs/msg/odometry.hpp>
#include <sensor_msgs/msg/detail/joint_state__struct.hpp>
#include <string>
#include <vector>
#include <cmath>
#include <functional>
#include <tf2/LinearMath/Matrix3x3.h>

#include <tf2/LinearMath/Quaternion.h>
#include <chrono>
#include <ObjectCreator.hpp>
#include <sensor_msgs/msg/joint_state.hpp>

//#include <pugixml.hpp>

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
        this->declare_parameter<double>("boat_velocity", 1.0);


        max_detection_radius_ = this->get_parameter("max_detection_radius").as_double();
        min_detection_radius_ = this->get_parameter("min_detection_radius").as_double();
        field_of_view_deg_ = this->get_parameter("field_of_view").as_double();
        int detection_rate = this->get_parameter("detection_rate").as_int();
        boat_velocity_ = this->get_parameter("boat_velocity").as_double();


        buoys_ = std::make_unique<ObjectCreator>(max_detection_radius_,min_detection_radius_,field_of_view_deg_);

        boats_ = std::make_unique<ObjectCreator>(max_detection_radius_,min_detection_radius_,field_of_view_deg_);
        boats_->add_object(20,20,0,"boat_1");
        boats_->add_object(20,20,0,"boat_2");
        //Add buoys to the map ("Hardcoded", must be same in the simulator)
        buoys_->add_objects_on_line(10,5,0,"Red",0,10.0f,10);
        buoys_->add_objects_on_line(10,10,0,"Green",0,10.0f,10);

        
        
        boat1_position_publisher_ = this->create_publisher<sensor_msgs::msg::JointState>("/boat1/position", 10);
        boat2_position_publisher_ = this->create_publisher<sensor_msgs::msg::JointState>("/boat2/position", 10);

        auto qos_geo_pose = rclcpp::QoS(rclcpp::KeepLast(1))
        .reliability(rclcpp::ReliabilityPolicy::BestEffort)
        .durability_volatile();

        position_subscriber_= this->create_subscription<nav_msgs::msg::Odometry>(
            "/sensors/odometry", qos_geo_pose, std::bind(&ObjectPublisher::position_cb, this, std::placeholders::_1));
        
        buoy_publisher_ = this->create_publisher<object_msgs::msg::Buoys>("/simulator/buoys",10);
        boat_publisher_ = this->create_publisher<object_msgs::msg::Boats>("/simulator/boats",10);
        
        buoy_timer_ = this->create_wall_timer(
            std::chrono::milliseconds(detection_rate),
            std::bind(&ObjectPublisher::publish_viewed_objects, this));
        boat_position_update_timer_ = this->create_wall_timer(
            std::chrono::milliseconds(10),
            std::bind(&ObjectPublisher::update_boat_position, this));
        }

//Usv position cb
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

    void update_boat_position(){
        // The following logic to move and rotate the boat is extremely hardcoded :<...
        static bool direction_boat_1;
        if (position_boat_1 >= 40.0){
            direction_boat_1 = true;
        }
        if (position_boat_1 <= 0.0){
            direction_boat_1 = false;
        }

        if(direction_boat_1){  //0.010 is deltatime (10ms),
            position_boat_1 -= boat_velocity_*0.010;
            angle_boat_1 = -90*M_PI/180;
        } else {                            
            position_boat_1 += boat_velocity_ * 0.010;
            angle_boat_1 = 90*M_PI/180 ;
        }
        boats_->set_object_position_y(0,-position_boat_1); // boat y position changes,  sign introduced to follow NED

        sensor_msgs::msg::JointState boat_1_pos;
        boat_1_pos.name = {"boat1/piston", "boat1/rotation"};
        boat_1_pos.position = {position_boat_1, angle_boat_1 };
        boat1_position_publisher_->publish(boat_1_pos);

        
        //sensor_msgs::msg::JointState boat_2_pos;
        //boat_2_pos.name = {"boat2/Joint1"};
        //boat_2_pos.position = {position_boat_2};
        //boat2_position_publisher_->publish(boat_2_pos);    
    }

    void publish_viewed_objects(){
        if(buoys_->try_get_viewed_objects_relative_position(buoy_objects_,current_position_.pose.pose.position.x,current_position_.pose.pose.position.y, current_heading_))
        {          object_msgs::msg::Buoys buoy_pub_msg;
            for(const auto &buoy: buoy_objects_){
                object_msgs::msg::Buoy buoy_msg;
                buoy_msg.pos_x = buoy.x; // NED: 
                buoy_msg.pos_y = buoy.y; // NED: 
                buoy_msg.pos_z = buoy.z; // NED: Depth // 2D distance
                buoy_msg.color = buoy.color;
                buoy_pub_msg.amount +=1;
                buoy_pub_msg.buoys.push_back(buoy_msg);
            }
            buoy_publisher_->publish(buoy_pub_msg);
        }

        if(boats_->try_get_viewed_objects_relative_position(boat_objects_,current_position_.pose.pose.position.x,current_position_.pose.pose.position.y, current_heading_))
        {
            object_msgs::msg::Boats boats_pub_msg;
            for(const auto &boat: boat_objects_){
                object_msgs::msg::Boat boat_msg;
                boat_msg.pos_x = boat.x; // NED: 
                boat_msg.pos_y = boat.y; // NED: 
                boat_msg.pos_z = boat.z; // NED: Depth // 2D distance
                boat_msg.color = boat.color;
                boats_pub_msg.amount +=1;
                boats_pub_msg.boats.push_back(boat_msg);
            }
            boat_publisher_->publish(boats_pub_msg);
        }
    }

private:
    double max_detection_radius_ = 20.0; //m
    double min_detection_radius_ = 0.6; //m
    double field_of_view_deg_=72.0; // deg

    double boat_velocity_ = 1; // m/s

    // Different objects to keep flexability with modification of fov and detection radius
    std::unique_ptr<ObjectCreator> buoys_; 
    std::vector<Object> buoy_objects_;
    
    std::unique_ptr<ObjectCreator> boats_; 
    std::vector<Object> boat_objects_;
    
    nav_msgs::msg::Odometry start_position_;
    nav_msgs::msg::Odometry current_position_;
    double current_heading_{};

    double position_boat_1;
    double angle_boat_1;
    double position_boat_2;
    
    rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr boat1_position_publisher_;
    rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr boat2_position_publisher_;

    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr position_subscriber_;
    rclcpp::Publisher<object_msgs::msg::Buoys>::SharedPtr buoy_publisher_;
    rclcpp::Publisher<object_msgs::msg::Boats>::SharedPtr boat_publisher_;

    rclcpp::TimerBase::SharedPtr buoy_timer_;
    rclcpp::TimerBase::SharedPtr boat_timer_;
    rclcpp::TimerBase::SharedPtr boat_position_update_timer_;
    
    

};

