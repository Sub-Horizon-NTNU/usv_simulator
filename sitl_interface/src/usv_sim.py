#!/usr/bin/env python3
import rclpy
from rclpy.node import Node

import socket
import struct
import json
import time 

from sensor_msgs.msg import JointState
from std_msgs.msg import Float64MultiArray

from sensor_msgs.msg import NavSatFix
from sensor_msgs.msg import Imu
from nav_msgs.msg import Odometry

from tf_transformations import quaternion_from_euler, euler_from_quaternion

import numpy as np

# From: https://github.com/bvibhav/stonefish_bluerov2/blob/master/scripts/ardusim_patch.py (19.01.2026) and modified.

class SimulatorConverter(Node):
    def __init__(self, node_name):
        super().__init__(node_name)

        # Subscribers
        self.create_subscription(Imu, "/sensors/imu", self._imu_callback, 1),
        self.create_subscription(NavSatFix, "/sensors/gps", self._gps_callback, 1),
        self.create_subscription(Odometry, "/sensors/compass", self._odom_callback, 1),

        # Publishers
        self.pub_pwm_thrusters = self.create_publisher(Float64MultiArray, "thruster_setpoints", 1)
        self.pub_pwm_servos    = self.create_publisher(JointState, "servo_setpoints", 1)
        
        # Publish everything
        self.timer = self.create_timer(1/800, self.looper)
        
        PORT = 9002

        self.sock_sitl = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock_sitl.bind(('', PORT))
        self.sock_sitl.settimeout(0.1)

        self.imu = None
        self.gps = None
        self.odom = None

    def _imu_callback(self, msg):
        self.imu = msg

    def _gps_callback(self, msg):
        self.gps = msg

    def _odom_callback(self, msg):
        self.odom = msg
    
    def _scale(self,x, in_min, in_max,  out_min,  out_max):
        y = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min
        return y

    def looper(self):
        if self.imu is None or self.odom is None:
            self.get_logger().info("Wating for callbacks", once=False)
            time.sleep(.001)
            return
        
        self.get_logger().info("Callbacks received", once=True)
        
        try:
            data, address = self.sock_sitl.recvfrom(100)
        except Exception as ex:
            self.get_logger().info("Socket receive failed, is SITL running?", once=False)
            time.sleep(0.001)
            return 
        
        parse_format = 'HHI16H'
        magic = 18458

        if len(data) != struct.calcsize(parse_format):
            print("got packet of len %u, expected %u" % (len(data), struct.calcsize(parse_format)))
            return 
        
        decoded = struct.unpack(parse_format,data)

        if magic != decoded[0]:
            print("Incorrect protocol magic %u should be %u" % (decoded[0], magic))
            return 

        frame_rate_hz = decoded[1]
        frame_count = decoded[2]
        #pwm = decoded[3:]
       
        pwm_thrusters = decoded[3:][0:4]
        pwm_servos =  decoded[3:][4:8]
        print("PWM Servo:",pwm_servos)
        print("PWM Thruster", pwm_thrusters)
        
        #Normalize servo commands
        pwm_thruster_setpoint = Float64MultiArray(data=[x for x in pwm_thrusters]) # Scale from 1100 to 1900 -> -1 to 1
        pwm_servos_setpoint = JointState()
        pwm_servos_setpoint.name = ["selene/Joint1","selene/Joint2","selene/Joint3","selene/Joint4"]
        pwm_servos_setpoint.position = [self._scale(x,500,2500,0,2*3.14) for x in pwm_servos] # Scale from 500 to1 2500 -> -1 to 1
        
        # Publish Servo commands
        self.pub_pwm_thrusters.publish(pwm_thruster_setpoint)
        self.pub_pwm_servos.publish(pwm_servos_setpoint)

        # Set messages
        accel = (self.imu.linear_acceleration.x, -self.imu.linear_acceleration.y, -self.imu.linear_acceleration.z)
        gyro = (self.imu.angular_velocity.x, -self.imu.angular_velocity.y, -self.imu.angular_velocity.z)
     
        pose_position = (
            self.odom.pose.pose.position.x,
            self.odom.pose.pose.position.y,
            self.odom.pose.pose.position.z
        )

        pose_attitude = euler_from_quaternion([
            self.odom.pose.pose.orientation.x,
            self.odom.pose.pose.orientation.y,
            self.odom.pose.pose.orientation.z,
            self.odom.pose.pose.orientation.w
        ])
        
        pose_attitude = [pose_attitude[0], pose_attitude[1], pose_attitude[2]]
        # print("Yaw:", np.rad2deg(pose_attitude[2]))
        
        twist_linear = (
            self.odom.twist.twist.linear.x,
            self.odom.twist.twist.linear.y,
            self.odom.twist.twist.linear.z,
            self.odom.twist.twist.angular.x,
            self.odom.twist.twist.angular.y,
            -self.odom.twist.twist.angular.z,
        )
        
        c_time = self.get_clock().now().to_msg()
        c_time = c_time.sec + c_time.nanosec/1e9

        # build JSON format
        IMU_fmt = {
            "gyro" : gyro,
            "accel_body" : accel
        }
        JSON_fmt = {
            "timestamp" : c_time,
            "imu" : IMU_fmt,
            "position" : pose_position,
            "attitude" : pose_attitude,
            "velocity" : twist_linear,                          
        }
       
        # print(self.gps.latitude)

        gps_data = {
        'time_usec' : int(c_time*1e6),#int(c_time/1e3),                        # (uint64_t) Timestamp (micros since boot or Unix epoch)
        'gps_id' : 0,                           # (uint8_t) ID of the GPS for multiple GPS inputs
        # 'ignore_flags' : 8,                     # (uint16_t) Flags indicating which fields to ignore (see GPS_INPUT_IGNORE_FLAGS enum). All other fields must be provided.
        # 'time_week_ms' : 0,                     # (uint32_t) GPS time (milliseconds from start of GPS week)
        # 'time_week' : 0,                        # (uint16_t) GPS week number
        'fix_type' : 5,                         # (uint8_t) 0-1: no fix, 2: 2D fix, 3: 3D fix. 4: 3D with DGPS. 5: 3D with RTK
        'lat' : int(self.gps.latitude*1e7),                              # (int32_t) Latitude (WGS84), in degrees * 1E7
        'lon' : int(self.gps.longitude*1e7),                              # (int32_t) Longitude (WGS84), in degrees * 1E7
        'alt' : 0,                              # (float) Altitude (AMSL, not WGS84), in m (positive for up)
         'hdop' : 1,                             # (float) GPS HDOP horizontal dilution of position in m
         'vdop' : 1,                             # (float) GPS VDOP vertical dilution of position in m
         'vn' : 0,                               # (float) GPS velocity in m/s in NORTH direction in earth-fixed NED frame
         've' : 0,                               # (float) GPS velocity in m/s in EAST direction in earth-fixed NED frame
         'vd' : 0,                               # (float) GPS velocity in m/s in DOWN direction in earth-fixed NED frame
         'speed_accuracy' : 0,                   # (float) GPS speed accuracy in m/s
         'horiz_accuracy' : 0,                   # (float) GPS horizontal accuracy in m
         'vert_accuracy' : 0,                    # (float) GPS vertical accuracy in m
         'satellites_visible' : 7                # (uint8_t) Number of satellites visible.
        }

        gps_data = json.dumps(gps_data)
        self.sock_sitl.sendto(gps_data.encode(), address)
        
        JSON_string = "\n" + json.dumps(JSON_fmt,separators=(',', ':')) + "\n"

        # Send to AP
        self.sock_sitl.sendto(bytes(JSON_string,"ascii"), address)

def main(args=None):
    rclpy.init(args=args)

    simulator_converter = SimulatorConverter(node_name="simulator_converter")
    
    rclpy.spin(simulator_converter)

    simulator_converter.destroy_node()
    rclpy.shutdown()

if __name__ == "__main__":
    main()
