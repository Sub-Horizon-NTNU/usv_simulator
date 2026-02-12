
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node

def generate_launch_description():
    max_detection_radius = LaunchConfiguration('max_detection_radius')
    min_detection_radius = LaunchConfiguration('min_detection_radius')
    field_of_view = LaunchConfiguration('field_of_view')
    detection_rate = LaunchConfiguration('detection_rate')

    max_detection_radius_arg = DeclareLaunchArgument('max_detection_radius',default_value='20.0')
    min_detection_radius_arg = DeclareLaunchArgument('min_detection_radius',default_value='0.60')
    field_of_view_arg = DeclareLaunchArgument('field_of_view',default_value='72.0')
    detection_rate_arg = DeclareLaunchArgument('detection_rate',default_value='100')

    object_publisher_node =  Node(
        package='object_publisher',
        executable='object_publisher',
        name='object_publisher',
        output='screen',
        parameters=[{
            'max_detection_radius': max_detection_radius,
            'min_detection_radius': min_detection_radius,
            'field_of_view': field_of_view,
            'detection_rate' : detection_rate
        }]
    )

    return LaunchDescription([
        max_detection_radius_arg,
        min_detection_radius_arg,
        field_of_view_arg,
        detection_rate_arg,
        object_publisher_node
    ])

