#setup
cmake_minimum_required(VERSION 3.10)
project(engine)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
include_directories(${PROJECT_SOURCE_DIR}/includes)

# variables - arrays with file list 
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    ${PROJECT_SOURCE_DIR}/src/*.cpp
    ${PROJECT_SOURCE_DIR}/src/*.c
)

file(GLOB_RECURSE IMGUI CONFIGURE_DEPENDS
    ${PROJECT_SOURCE_DIR}/includes/imgui/*.cpp
)

# source files and libs
add_executable(engine ${SOURCES})
add_library(imgui STATIC ${IMGUI})

# link libs
target_link_directories(engine PRIVATE ${PROJECT_SOURCE_DIR}/libs) # check this for the file while linking
find_package(OpenGL REQUIRED)

target_link_libraries(engine PRIVATE libglfw3.a OpenGL::GL imgui)