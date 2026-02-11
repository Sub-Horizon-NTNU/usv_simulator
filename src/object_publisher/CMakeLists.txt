cmake_minimum_required(VERSION 3.8)
project(object_publisher)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS on)
# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)

find_package(mavros_msgs REQUIRED)
find_package(stonefish_ros2 REQUIRED)
find_package(object_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(tf2_ros REQUIRED)


include(FetchContent)

FetchContent_Declare(
  pugixml
  GIT_REPOSITORY https://github.com/zeux/pugixml.git
  GIT_TAG v1.15  
)
FetchContent_MakeAvailable(pugixml)


add_compile_definitions(PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}")


add_executable(object_publisher src/main.cpp)
target_include_directories(object_publisher PRIVATE include)
ament_target_dependencies(object_publisher 
  rclcpp
  mavros_msgs
  stonefish_ros2  
  object_msgs
  nav_msgs
  tf2
  tf2_ros
  tf2_geometry_msgs
)
target_link_libraries(object_publisher pugixml::pugixml)

install(TARGETS object_publisher
  DESTINATION lib/${PROJECT_NAME}
)


ament_package()
