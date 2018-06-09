cmake_minimum_required (VERSION 2.8)

project(SeamCarving)
aux_source_directory(. DIR_SRCS)
add_definitions(-std=c++11)
if(APPLE)
set(CMAKE_C_COMPILER "/opt/local/bin/gcc-mp-8")
set(CMAKE_CXX_COMPILER "/opt/local/bin/g++-mp-8")
endif()

add_executable(SeamCarving ${DIR_SRCS})
