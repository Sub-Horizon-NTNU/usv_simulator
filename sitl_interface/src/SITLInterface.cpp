#include "SITLInterface.hpp"


    SITLInterface::SITLInterface():  Node("sitl_interface") {
        RCLCPP_INFO(this->get_logger(),"SITL Interface started");
        

        this->declare_parameter("ip_address","127.0.0.1");
        
        imu_sub_ = this->create_subscription<sensor_msgs::msg::Imu>("/sensors/imu",
            10,std::bind(&SITLInterface::imu_callback,this,std::placeholders::_1));

        odo_sub_ = this->create_subscription<nav_msgs::msg::Odometry>("/sensors/odometry",
            10,std::bind(&SITLInterface::odo_callback,this,std::placeholders::_1));

        //gps_sub_ = this->create_subscription<sensor_msgs::msg::NavSatFix>("/sensors/gps",
        //     10,std::bind(&SITLInterface::gps_callback,this,std::placeholders::_1));
        
        update_timer_ = this->create_wall_timer(
            std::chrono::milliseconds(10), std::bind(&SITLInterface::update, this));
        
        std::string ip_address = this->get_parameter("ip_address").as_string();
        udp_client_ = std::make_unique<UDPClient>(ip_address,9002);
        
        thruster_pub_ = this->create_publisher<std_msgs::msg::Float64MultiArray>("thruster_setpoints",10);
        servo_pub_ =  this->create_publisher<sensor_msgs::msg::JointState>("servo_setpoints",10);
    }

    void SITLInterface::imu_callback(const sensor_msgs::msg::Imu::SharedPtr msg){
       
        auto q_msg = msg->orientation;
        tf2::Quaternion quat(q_msg.x, q_msg.y, q_msg.z, q_msg.w);
        tf2::Matrix3x3 m(quat);

        m.getRPY(rpy[0],rpy[1],rpy[2]);
        //   +
        //double c_time = this->get_clock()->now().seconds(); //+ this->get_clock()->now().nanoseconds() * 1e-9;
        
        simulator_data_.attitude[0] = rpy[0];
        simulator_data_.attitude[1] = rpy[1];
        simulator_data_.attitude[2] = rpy[2];

        simulator_data_.gyro[0] = msg->angular_velocity.x;
        simulator_data_.gyro[1] = msg->angular_velocity.y;
        simulator_data_.gyro[2] = msg->angular_velocity.z;

        simulator_data_.accel_body[0] = msg->linear_acceleration.x;
        simulator_data_.accel_body[1] = msg->linear_acceleration.y;
        simulator_data_.accel_body[2] = msg->linear_acceleration.z;
    }

    void SITLInterface::odo_callback(const nav_msgs::msg::Odometry::SharedPtr msg){

        //simulator_data_.timestamp = (rclcpp::Time(msg->header.stamp).nanoseconds()) / 1.0e9;
        simulator_data_.position[0] = msg->pose.pose.position.x ;
        simulator_data_.position[1] = msg->pose.pose.position.y;
        simulator_data_.position[2] = msg->pose.pose.position.z;

        simulator_data_.velocity[0] = msg->twist.twist.linear.x;
        simulator_data_.velocity[1] = msg->twist.twist.linear.y;
        simulator_data_.velocity[2] = msg->twist.twist.linear.z;

    }

    void SITLInterface::gps_callback(const sensor_msgs::msg::NavSatFix::SharedPtr msg){
        
        const auto now = this->get_clock()->now();
        const double t = now.seconds(); 

        gps_data_.time_usec = static_cast<uint64_t>(t * 1e6);
        gps_data_.gps_id = 0;
        gps_data_.fix_type = 5;
        gps_data_.lat = msg->latitude  * 1e7;
        gps_data_.lon = msg->longitude * 1e7;
            
        gps_data_.alt = 0.0f;

        gps_data_.hdop = 1.0f;
        gps_data_.vdop = 1.0f;

        gps_data_.vn = 0.0f;
        gps_data_.ve = 0.0f;
        gps_data_.vd = 0.0f;

        gps_data_.speed_accuracy = 0.0f;
        gps_data_.horiz_accuracy = 0.0f;
        gps_data_.vert_accuracy  = 0.0f;

        gps_data_.satellites_visible = 12;
    }


    void SITLInterface::update(){

        static sitl_data incoming;

        //capture by ref and update simulator and send states only if control data is successfully received.
        if(udp_client_->try_receive_sitl(incoming)){
            auto servo_msg = sensor_msgs::msg::JointState();
            auto thruster_msg = std_msgs::msg::Float64MultiArray();
            servo_msg.position.resize(4); 
            thruster_msg.data.resize(4);
            
            servo_msg.name = {"selene/Joint1","selene/Joint2","selene/Joint3","selene/Joint4"};
            servo_msg.header.stamp = this->now();
            
            for(int i = 0; i < 4;  i++){ 
                thruster_msg.data[i] = scale(static_cast<double>(incoming.pwm[i]),1100, 1900,1100,1900); // Bluerobotics basic ESC 1100-1900µs
            }
            
            for(int i=0; i <4; i++){
                servo_msg.position[i] = scale(static_cast<double>(incoming.pwm[i+4]),500,2500,0,2*M_PI); // Stonefish servos doesnt take in pulse duration in µs, it takes in rad the real ones operate with pulse duration of  500µs - 2500µs.
            }
          
            thruster_pub_->publish(thruster_msg);
            servo_pub_->publish(servo_msg);
            
            c_time = this->get_clock()->now().nanoseconds() * 1e-9  - 1764420656; //  System time displayed on QGC was 2082, so time until 2082 is subtracted :D, fixed a issue with mavros.
            //RCLCPP_INFO(this->get_logger(), "c_time :%f ", c_time);
            simulator_data_.timestamp = c_time;

            to_json(sim_data_j, simulator_data_);

            // GPS commented out

            //to_json(gps_data_j,gps_data_);
            
            //std::string gps_data_string = gps_data_j.dump();
            std::string data_string = "\n" + sim_data_j.dump() + "\n";
            
            //udp_client_->send_string(gps_data_string);
            udp_client_->send_string(data_string);
            
        }
    }

    inline float SITLInterface::scale(float x, float in_min, float in_max, float out_min, float out_max)
    {
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }

