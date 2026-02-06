from launch_ros.actions import Node
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution



def generate_launch_description():
    
    ip_address_arg = DeclareLaunchArgument(
            "ip_address", default_value = "127.0.0.1"
        )

    sitl_interface = Node(
        package='sitl_interface',
        executable='sitl_interface', 
        output='screen',
        parameters = [{"ip_address" : LaunchConfiguration("ip_address")}]
    )

    return LaunchDescription([
        ip_address_arg,
        sitl_interface
    ])