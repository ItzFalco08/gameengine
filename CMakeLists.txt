#setup
cmake_minimum_required(VERSION 3.10)
project(engine)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
include_directories(${PROJECT_SOURCE_DIR}/includes)

# Make the project root available as a macro in code
file(TO_CMAKE_PATH "${PROJECT_SOURCE_DIR}" PROJECT_ROOT_NORMALIZED)
add_definitions(-DPROJECT_ROOT="${PROJECT_ROOT_NORMALIZED}")

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

target_link_libraries(engine PRIVATE glfw3.lib OpenGL::GL imgui)