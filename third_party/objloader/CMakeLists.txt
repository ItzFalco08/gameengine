cmake_minimum_required(VERSION 3.14)
project(objloader)
add_library(objloader INTERFACE)
target_include_directories(objloader INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
