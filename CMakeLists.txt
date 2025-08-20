cmake_minimum_required(VERSION 3.28)

project(gl
        VERSION 0.1.0
        LANGUAGES CXX
        DESCRIPTION "Learning OpenGL")

set(CMAKE_COLOR_DIAGNOSTICS ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB LIB_SOURCES CONFIGURE_DEPENDS lib/*.cxx)
add_library(lib)
target_sources(lib
  PUBLIC FILE_SET modules TYPE CXX_MODULES
  FILES ${LIB_SOURCES})
target_link_libraries(lib
  PUBLIC glfw
  PUBLIC glm::glm
  PUBLIC GLEW::GLEW)

find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

file(GLOB BIN_SOURCES CONFIGURE_DEPENDS bin/*.cpp)
foreach(bin_src ${BIN_SOURCES})
  get_filename_component(bin_name ${bin_src} NAME_WE)
  add_executable(${bin_name} ${bin_src})
  target_link_libraries(${bin_name} PRIVATE lib)
endforeach()
