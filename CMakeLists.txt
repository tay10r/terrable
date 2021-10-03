cmake_minimum_required(VERSION 3.9.6)

project(terrable)

include(FetchContent)

find_package(Qt5 REQUIRED COMPONENTS Widgets)
find_package(OpenMP REQUIRED)

FetchContent_Declare(bvh URL "https://github.com/madmann91/bvh/archive/master.zip")

FetchContent_MakeAvailable(bvh)

set(tools
  tools/ambient_occlusion_tool.hpp
  tools/ambient_occlusion_tool.cpp
  tools/height_map_import_tool.hpp
  tools/height_map_import_tool.cpp
  tools/scale_tool.hpp
  tools/scale_tool.cpp)

add_executable(terrable
  main.cpp
  core/terrain.hpp
  core/terrain.cpp
  core/tool.hpp
  core/tool.cpp
  core/camera.hpp
  core/camera.cpp
  core/terrain_rt_model.hpp
  core/terrain_rt_model.cpp
  core/texture.hpp
  core/texture.cpp
  terrain_history.hpp
  terrain_history.cpp
  terrain_view.hpp
  terrain_view.cpp
  orbit_camera.hpp
  orbit_camera.cpp
  tool_list.hpp
  tool_list.cpp
  tool_list_entry.hpp
  tool_list_entry.cpp
  ${tools}
  resources.qrc)

if(NOT MSVC)
  target_compile_options(terrable PRIVATE -Wall -Wextra -Werror -Wfatal-errors -Wno-unknown-pragmas)
endif(NOT MSVC)

set_target_properties(terrable
  PROPERTIES
    AUTOMOC ON
    AUTORCC ON)

target_compile_features(terrable PRIVATE cxx_std_17)

target_include_directories(terrable
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(terrable
  PRIVATE
    Qt5::Widgets
    OpenMP::OpenMP_CXX
    bvh)
