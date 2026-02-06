from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription, TimerAction
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution

def generate_launch_description():
    stonefish_sim_dir = get_package_share_directory("usv_simulator")
    stonefish_ros2_dir = get_package_share_directory("stonefish_ros2")
    sitl_interface_dir = get_package_share_directory("sitl_interface")
    ardupilot_sitl_dir = get_package_share_directory("ardupilot_sitl")
    mavros_dir = get_package_share_directory("mavros")

    simulation_data_arg = DeclareLaunchArgument(
        "simulation_data",
        default_value=PathJoinSubstitution([stonefish_sim_dir, "data/drones"]),
        description="Path to the simulation data folder",
    )
    
    scenario_desc_arg = DeclareLaunchArgument(
        "scenario_desc",
        description="Path to the scenario file (.scn)",
        default_value=PathJoinSubstitution([stonefish_sim_dir, "data/main.scn"])
    )

    window_res_x_arg = DeclareLaunchArgument(
        "window_res_x", default_value="1920", description="Window width"
    )

    window_res_y_arg = DeclareLaunchArgument(
        "window_res_y", default_value="1080", description="Window height"
    )

    quality_arg = DeclareLaunchArgument(
        "rendering_quality", default_value="high"
    )

    ip_address_arg = DeclareLaunchArgument(
            'ip_address', default_value="127.0.0.1"
    )
    
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


    fcu_url_arg = DeclareLaunchArgument("fcu_url",default_value="udp://127.0.0.1:14550@14555")
    gcs_url_arg = DeclareLaunchArgument("gcs_url",default_value="udp://@127.0.0.1:14560")
    mavros_launch = IncludeLaunchDescription(
       PythonLaunchDescriptionSource(
           PathJoinSubstitution([mavros_dir, "launch/apm.launch"])
       ),
       launch_arguments={
           "fcu_url": LaunchConfiguration("fcu_url"),
           "gcs_url": LaunchConfiguration("gcs_url"),
       }.items()
   )


    return LaunchDescription([
        simulation_data_arg,
        scenario_desc_arg,
        window_res_x_arg,
        window_res_y_arg,
        quality_arg,
        ip_address_arg,
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
        include_stonefish_launch,
        delayed_ardupilot_sitl_launch,
        delayed_sitl_interface_launch,
        fcu_url_arg,
        gcs_url_arg,
        mavros_launch
    ])
