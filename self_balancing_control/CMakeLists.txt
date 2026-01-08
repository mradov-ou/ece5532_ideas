cmake_minimum_required(VERSION 3.5)
project(self_balancing_control)
set(CMAKE_CXX_STANDARD 20)

find_package(ament_cmake_auto REQUIRED)
ament_auto_find_build_dependencies()

install(
  DIRECTORY
    launch
  DESTINATION share/${PROJECT_NAME}
)

ament_auto_package()