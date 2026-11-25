cmake_minimum_required(VERSION 3.8)
project(semantic_nav_mapper)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()
set(CMAKE_CXX_STANDARD 17)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(vision_msgs REQUIRED)
find_package(visualization_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(semantic_nav_interfaces REQUIRED)

# Pure geometry / association code, no ROS: easy to unit test.
add_library(semantic_geometry src/geometry.cpp)
target_include_directories(semantic_geometry PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)

add_executable(object_mapper_node src/object_mapper_node.cpp)
target_link_libraries(object_mapper_node semantic_geometry)
ament_target_dependencies(object_mapper_node
  rclcpp sensor_msgs geometry_msgs vision_msgs visualization_msgs
  tf2 tf2_ros tf2_geometry_msgs semantic_nav_interfaces)

install(TARGETS object_mapper_node DESTINATION lib/${PROJECT_NAME})
install(TARGETS semantic_geometry ARCHIVE DESTINATION lib LIBRARY DESTINATION lib)
install(DIRECTORY include/ DESTINATION include)

if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  ament_add_gtest(test_geometry test/test_geometry.cpp)
  target_link_libraries(test_geometry semantic_geometry)
endif()

ament_package()
