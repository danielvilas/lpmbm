# Specify the minimum version for CMake

cmake_minimum_required(VERSION 2.8)

# Project's name

project(lpm_benchmark)
# Set the output folder where your program will be created
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(PROJECT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)

# The following folder will be included
include_directories("${PROJECT_SOURCE_DIR}")

add_executable(lpm src/ina219.c src/Monitor.c src/proccommon.c src/ProcConnector.c src/procnetstat.c src/procstat.c)

install (TARGETS lpm DESTINATION bin)

set_property(TARGET lpm PROPERTY C_STANDARD 99)
