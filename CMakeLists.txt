cmake_minimum_required(VERSION 3.9.6)

project(terrable)

find_package(Qt5 REQUIRED COMPONENTS Widgets)
find_package(OpenMP REQUIRED)

add_executable(terrable
  main.cpp
  terrain.hpp
  terrain.cpp
  terrain_history.hpp
  terrain_history.cpp
  terrain_view.hpp
  terrain_view.cpp
  camera.hpp
  camera.cpp
  orbit_camera.hpp
  orbit_camera.cpp
  tool.hpp
  tool.cpp
  tool_list.hpp
  tool_list.cpp
  tool_list_entry.hpp
  tool_list_entry.cpp
  scale_tool.hpp
  scale_tool.cpp
  height_map_import_tool.hpp
  height_map_import_tool.cpp
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
    Qt5::Widgets
    OpenMP::OpenMP_CXX)
