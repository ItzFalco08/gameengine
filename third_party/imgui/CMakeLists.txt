cmake_minimum_required(VERSION 3.14)
project(imgui)
add_library(imgui STATIC
    imgui.cpp
    imgui_draw.cpp
    imgui_tables.cpp
    imgui_widgets.cpp
    imgui_demo.cpp

    backends/imgui_impl_opengl3.cpp
    backends/imgui_impl_sdl3.cpp
)

target_include_directories(imgui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(imgui PUBLIC SDL3::SDL3-static)
