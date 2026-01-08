cmake_minimum_required(VERSION 3.5)
project(igvc_bag_processing)
set(CMAKE_CXX_STANDARD 20)

find_package(ament_cmake_auto REQUIRED)
ament_auto_find_build_dependencies()

ament_auto_add_executable(pinhole_geometry_example
  src/pinhole_geometry_example.cpp
  src/PinholeGeometry.cpp
)

install(
  DIRECTORY
    launch
    rviz
  DESTINATION share/${PROJECT_NAME}
)

ament_auto_package()