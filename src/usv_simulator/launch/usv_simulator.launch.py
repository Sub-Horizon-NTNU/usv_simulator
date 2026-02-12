from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription, TimerAction, ExecuteProcess, RegisterEventHandler
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution
from launch_xml.launch_description_sources import XMLLaunchDescriptionSource
import xacro
import subprocess
import os

stonefish_sim_dir = get_package_share_directory("usv_simulator")
stonefish_ros2_dir = get_package_share_directory("stonefish_ros2")
sitl_interface_dir = get_package_share_directory("sitl_interface")
ardupilot_sitl_dir = get_package_share_directory("ardupilot_sitl")
object_publisher_dir = get_package_share_directory("object_publisher")
mavros_dir = get_package_share_directory("mavros")
xacro_path = os.path.join(stonefish_sim_dir, "data", "main.scn.xacro")
scn_path   = os.path.join(stonefish_sim_dir, "data", "main.scn")

subprocess.run(["xacro", xacro_path, "-o", scn_path], check=True) # Generate scn from xacro files

def generate_launch_description():
    simulation_data_arg = DeclareLaunchArgument(
        "simulation_data",
        default_value=PathJoinSubstitution([stonefish_sim_dir, "data/drones"]),
        description="Path to the simulation data folder",
    )
    
    scenario_desc_arg = DeclareLaunchArgument(
        "scenario_desc",
        description="Path to the scenario file (.scn)",
        default_value=scn_path
    )

    window_res_x_arg = DeclareLaunchArgument("window_res_x", default_value="1920", description="Window width" )
    window_res_y_arg = DeclareLaunchArgument("window_res_y", default_value="1080", description="Window height")
    quality_arg = DeclareLaunchArgument("rendering_quality", default_value="high")


    #include stonefish simulator launch
    include_stonefish_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            PathJoinSubstitution([stonefish_ros2_dir, "launch/stonefish_simulator.launch.py"])
        ),
        launch_arguments={
            "simulation_data": LaunchConfiguration("simulation_data"),
            "scenario_desc": LaunchConfiguration("scenario_desc"),
            "window_res_x": LaunchConfiguration("window_res_x"),
            "window_res_y": LaunchConfiguration("window_res_y"),
            "rendering_quality": LaunchConfiguration("rendering_quality"),
        }.items(),
    )

    stonefish_launch_list = [
        simulation_data_arg,
        scenario_desc_arg,
        window_res_x_arg,
        window_res_y_arg,
        quality_arg,
        include_stonefish_launch
    ]


#Ardupilot SITL DDS launch, the parameters in the SITL class had to be overwritten for it to work. should be fixed in a future ardupilot release.
# Also note that the DDS is most likely not used, but its implementation still remains available in case it is deemed practical.
    
    # SITL Interface   
    ip_address_arg = DeclareLaunchArgument('ip_address', default_value="127.0.0.1")
    

    include_sitl_interface_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            PathJoinSubstitution([sitl_interface_dir, "launch/sitl_interface.launch.py"])
            ),
        launch_arguments={ 
            "ip_address" : LaunchConfiguration("ip_address")
            }.items()
    )
    
    delayed_sitl_interface_launch = TimerAction(
        period=10.0,
        actions=[include_sitl_interface_launch]
    )


    sitl_interface_launch_list = [
        ip_address_arg,
        delayed_sitl_interface_launch
    ]

    transport_arg = DeclareLaunchArgument("transport", default_value="udp4")
    synthetic_clock_arg = DeclareLaunchArgument("synthetic_clock", default_value="False")
    wipe_arg = DeclareLaunchArgument("wipe",default_value="False")
    instance_arg = DeclareLaunchArgument("instance", default_value="0")
    model_arg = DeclareLaunchArgument("model",default_value="JSON")
    speedup_arg = DeclareLaunchArgument("speedup", default_value="1")
    master_arg = DeclareLaunchArgument("master",default_value="tcp:127.0.0.1:5760")
    home_arg = DeclareLaunchArgument("home",default_value="AALESUND")
    defaults_arg = DeclareLaunchArgument("defaults",default_value= PathJoinSubstitution([ardupilot_sitl_dir,"config","default_params","rover.parm"])) # Rover parameters
    sim_address_arg = DeclareLaunchArgument("sim_address", default_value="127.0.0.1") # 
    slave_arg = DeclareLaunchArgument("slave", default_value="0")

    include_ardupilot_sitl_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            PathJoinSubstitution([ardupilot_sitl_dir, "launch/sitl_dds_udp.launch.py"])
        ),
        launch_arguments={
            "transport" : LaunchConfiguration("transport"),
            "synthetic_clock" : LaunchConfiguration("synthetic_clock"),
            "wipe" : LaunchConfiguration("wipe"),
            "model" : LaunchConfiguration("model"),
            "speedup" : LaunchConfiguration("speedup"),
            "master" : LaunchConfiguration("master") ,
            "home" : LaunchConfiguration("home"), 
            "instance" : LaunchConfiguration("instance"),
            "defaults" : LaunchConfiguration("defaults"),
            "sim_address" : LaunchConfiguration("sim_address"),
            "slave" : LaunchConfiguration("slave")
        }.items()
    )

    delayed_ardupilot_sitl_launch = TimerAction(
        period=0.0,
        actions=[include_ardupilot_sitl_launch]
    )

    sitl_launch_list = [
        transport_arg,
        synthetic_clock_arg,
        wipe_arg,
        instance_arg,
        model_arg,
        speedup_arg,
        master_arg,
        home_arg,
        defaults_arg,
        sim_address_arg,
        slave_arg,
        delayed_ardupilot_sitl_launch
    ]

## Mavros:

    fcu_url_arg = DeclareLaunchArgument("fcu_url",default_value="udp://127.0.0.1:14550@14555")
    gcs_url_arg = DeclareLaunchArgument("gcs_url",default_value="udp://@127.0.0.1:14560")
    mavros_launch = IncludeLaunchDescription(
       XMLLaunchDescriptionSource(
           PathJoinSubstitution([mavros_dir, "launch/apm.launch"])
       ),
       launch_arguments={
           "fcu_url": LaunchConfiguration("fcu_url"),
           "gcs_url": LaunchConfiguration("gcs_url"),
       }.items()
    )
    
    delayed_mavros_launch = TimerAction(
        period=15.0,
        actions=[mavros_launch]
    )

    mavros_launch_list = [
        fcu_url_arg,
        gcs_url_arg,
        delayed_mavros_launch,
    ]
 
## Object publisher node:
    max_detection_radius_arg = DeclareLaunchArgument('max_detection_radius',default_value='20.0')
    min_detection_radius_arg = DeclareLaunchArgument('min_detection_radius',default_value='0.60')
    field_of_view_arg =        DeclareLaunchArgument('field_of_view',default_value='72.0')
    detection_rate_arg =       DeclareLaunchArgument('detection_rate',default_value='100')

    object_publisher_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
                PathJoinSubstitution([object_publisher_dir,"launch/object_publisher.launch.py"])
            ),
            launch_arguments = {
                "max_detection_radius" : LaunchConfiguration("max_detection_radius"),
                "min_detection_radius" : LaunchConfiguration("min_detection_radius"),
                "field_of_view" : LaunchConfiguration("field_of_view"),
                "detection_rate" : LaunchConfiguration("detection_rate")
            }.items()
    )
    

    object_publisher_launch_list = [
        max_detection_radius_arg,
        min_detection_radius_arg,
        field_of_view_arg,
        detection_rate_arg,
        object_publisher_launch
    ]

# Combine all nodes
    system_launch = (
        stonefish_launch_list +
        sitl_interface_launch_list +
        sitl_launch_list +
        mavros_launch_list +
        object_publisher_launch_list
    )

    return LaunchDescription(system_launch)
