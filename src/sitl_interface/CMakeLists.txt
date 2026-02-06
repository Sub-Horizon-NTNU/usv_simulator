cmake_minimum_required(VERSION 3.8)
project(sitl_interface)

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(tf2_ros REQUIRED)


include(FetchContent)
FetchContent_Declare(
  asio
  GIT_REPOSITORY https://github.com/chriskohlhoff/asio
  GIT_TAG        asio-1-36-0
)

 

FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.12.0
)

FetchContent_MakeAvailable(asio nlohmann_json)

add_executable(sitl_interface 
  src/main.cpp
  src/UDPClient.cpp
  src/SITLInterface.cpp
  src/Data.cpp
)

target_include_directories(sitl_interface
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

ament_target_dependencies(sitl_interface 
rclcpp 
std_msgs 
sensor_msgs
nav_msgs
tf2_geometry_msgs
tf2_ros
)

target_link_libraries(sitl_interface
 nlohmann_json::nlohmann_json

)

target_compile_definitions(sitl_interface PUBLIC ASIO_STANDALONE)

install(TARGETS sitl_interface
DESTINATION lib/${PROJECT_NAME})

install(DIRECTORY
  launch
  DESTINATION share/${PROJECT_NAME}
)

install(DIRECTORY include/
  DESTINATION include)


ament_package()
