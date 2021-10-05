cmake_minimum_required(VERSION 3.9.6)

project(terrable_terrain_view)

find_package(Qt5 REQUIRED COMPONENTS Widgets)

add_library(terrable_terrain_view
  QTerrainView/qterrainview.h
  private/qterrainview.cpp
  private/open_gl_widget.hpp
  private/open_gl_widget.cpp
  private/open_gl_terrain.hpp
  private/open_gl_terrain.cpp
  private/pbr_texture.h
  private/pbr_texture.cpp
  private/palette.h
  private/palette.cpp
  private/elevation_converter.h
  private/elevation_converter.cpp)

if(NOT MSVC)
  target_compile_options(terrable_terrain_view PRIVATE -Wall -Wextra -Werror -Wfatal-errors)
endif(NOT MSVC)

target_link_libraries(terrable_terrain_view
  PUBLIC
    Qt5::Widgets)

set_target_properties(terrable_terrain_view
  PROPERTIES
    AUTOMOC ON)

target_include_directories(terrable_terrain_view
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/QTerrainView")

add_library(terrable::terrain_view ALIAS terrable_terrain_view)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)

  add_executable(main
    private/main.cpp)

  target_link_libraries(main terrable::terrain_view)

endif(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
