cmake_minimum_required(VERSION 3.8)
project(usv_simulator)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(stonefish_ros2 REQUIRED)

install(DIRECTORY
  launch
  data
  DESTINATION share/${PROJECT_NAME}
)


ament_package()
