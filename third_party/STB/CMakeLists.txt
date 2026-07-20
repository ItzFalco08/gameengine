cmake_minimum_required(VERSION 3.14)
project(STB)
add_library(STB INTERFACE)
target_include_directories(STB INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
