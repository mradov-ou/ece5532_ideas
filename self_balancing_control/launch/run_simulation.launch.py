import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.actions import Node


def generate_launch_description():

    gazebo_sim = IncludeLaunchDescription(
        PythonLaunchDescriptionSource([
            os.path.join(get_package_share_directory('teeterbot_gazebo'), 'launch', 'gazebo_bringup.launch.py')
        ]),
        launch_arguments={
            'world_sdf_file': os.path.join(get_package_share_directory('teeterbot_gazebo'), 'worlds', 'teeterbot_empty_world.sdf'),
            'gz_bridge_file': os.path.join(get_package_share_directory('teeterbot_gazebo'), 'config', 'ros_gz_bridge.yaml'),
        }.items()
    )

    return LaunchDescription([
        gazebo_sim
    ])
