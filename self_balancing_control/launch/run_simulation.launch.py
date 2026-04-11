import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription, DeclareLaunchArgument
from launch.conditions import IfCondition, UnlessCondition
from launch.substitutions import LaunchConfiguration
from launch.launch_description_sources import AnyLaunchDescriptionSource, PythonLaunchDescriptionSource
from launch_ros.actions import Node


def generate_launch_description():

    training_wheels = LaunchConfiguration('training_wheels')

    teeterbot = IncludeLaunchDescription(
        AnyLaunchDescriptionSource(
            os.path.join(get_package_share_directory('teeterbot_gazebo'), 'launch', 'teeterbot.launch.xml')
        ),
        condition=UnlessCondition(training_wheels),
    )

    teeterbot_training_wheels = IncludeLaunchDescription(
        AnyLaunchDescriptionSource(
            os.path.join(get_package_share_directory('teeterbot_gazebo'), 'launch', 'teeterbot_training_wheels.launch.xml')
        ),
        condition=IfCondition(training_wheels),
    )

    return LaunchDescription([
        DeclareLaunchArgument('training_wheels', default_value='false', description='Prevent Teeterbot from falling over'),
        teeterbot,
        teeterbot_training_wheels,
    ])
