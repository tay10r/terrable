cmake_minimum_required(VERSION 3.9.6)

project(terrable)

find_package(Qt5 REQUIRED COMPONENTS Widgets)
find_package(glm REQUIRED)

add_executable(terrable
  main.cpp
  terrain.hpp
  terrain.cpp
  terrain_view.hpp
  terrain_view.cpp
  camera.hpp
  camera.cpp
  orbit_camera.hpp
  orbit_camera.cpp
  resources.qrc)

if(NOT MSVC)
  target_compile_options(terrable PRIVATE -Wall -Wextra -Werror -Wfatal-errors)
endif(NOT MSVC)

set_target_properties(terrable
  PROPERTIES
    AUTOMOC ON
    AUTORCC ON)

target_link_libraries(terrable
  PRIVATE
    Qt5::Widgets glm::glm)
