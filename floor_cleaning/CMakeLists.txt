cmake_minimum_required(VERSION 3.5)
project(floor_cleaning)

find_package(ament_cmake REQUIRED)

install(
  DIRECTORY
    launch
    rviz
    worlds
  DESTINATION share/${PROJECT_NAME}
)
ament_package()