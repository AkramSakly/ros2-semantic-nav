"""Full stack in simulation.

    ros2 launch semantic_nav_bringup sim.launch.py

Starts: Gazebo (TurtleBot3 waffle, house world) + SLAM Toolbox + Nav2 + RViz
        + detector_node (YOLOv8) + object_mapper_node (C++).
Then, in another terminal:
    ros2 run semantic_nav_commander go_to_object chair
"""

import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import (DeclareLaunchArgument, IncludeLaunchDescription,
                            SetEnvironmentVariable, TimerAction)
from launch.conditions import IfCondition
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def generate_launch_description():
    bringup = get_package_share_directory('semantic_nav_bringup')
    tb3_gazebo = get_package_share_directory('turtlebot3_gazebo')
    nav2 = get_package_share_directory('nav2_bringup')

    use_rviz = LaunchConfiguration('use_rviz')
    use_perception = LaunchConfiguration('use_perception')
    nav2_params = LaunchConfiguration('nav2_params')

    args = [
        DeclareLaunchArgument('use_rviz', default_value='true'),
        DeclareLaunchArgument('use_perception', default_value='true',
                              description='Start detector_node and object_mapper_node'),
        DeclareLaunchArgument('nav2_params',
                              default_value=os.path.join(nav2, 'params', 'nav2_params.yaml'),
                              description='TODO(M1): tune a copy in config/ for the TurtleBot3'),
    ]

    # The waffle model is the TurtleBot3 that carries a camera.
    env = SetEnvironmentVariable('TURTLEBOT3_MODEL', 'waffle')

    # TODO(M1): the house world has few objects YOLO knows well. Make your own world in
    #   worlds/ with chairs, plants, a couch... (Gazebo model database) and launch it here.
    gazebo = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(tb3_gazebo, 'launch', 'turtlebot3_house.launch.py')))

    nav = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(os.path.join(nav2, 'launch', 'bringup_launch.py')),
        launch_arguments={
            'slam': 'True',
            'map': '',
            'use_sim_time': 'True',
            'params_file': nav2_params,
        }.items())

    rviz = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(os.path.join(nav2, 'launch', 'rviz_launch.py')),
        condition=IfCondition(use_rviz))
    # TODO(M4): save your own RViz config (map + detections image + /semantic_map/markers)
    #   in config/semantic_nav.rviz and start rviz2 with it instead.

    detector = Node(
        package='semantic_nav_detector', executable='detector_node', output='screen',
        parameters=[os.path.join(bringup, 'config', 'detector.yaml')],
        condition=IfCondition(use_perception))

    mapper = Node(
        package='semantic_nav_mapper', executable='object_mapper_node', output='screen',
        parameters=[os.path.join(bringup, 'config', 'mapper.yaml')],
        condition=IfCondition(use_perception))

    return LaunchDescription(args + [
        env,
        gazebo,
        # Give Gazebo time to spawn the robot before Nav2 looks for TF.
        TimerAction(period=5.0, actions=[nav, rviz, detector, mapper]),
    ])
