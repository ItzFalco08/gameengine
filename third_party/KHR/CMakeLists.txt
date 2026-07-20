cmake_minimum_required(VERSION 3.14)
project(KHR)
add_library(KHR INTERFACE)
target_include_directories(KHR INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
