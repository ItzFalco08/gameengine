cmake_minimum_required(VERSION 3.14)

project(glad)

add_library(glad STATIC ${CMAKE_CURRENT_SOURCE_DIR}/gl.c)

target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
