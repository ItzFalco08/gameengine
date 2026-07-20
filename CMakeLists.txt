cmake_minimum_required(VERSION 3.14)
project(engine)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# source files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/src/*.cpp)

# libraries
add_subdirectory(third_party/glfw)
add_subdirectory(third_party/imgui)
add_subdirectory(third_party/glm)
add_subdirectory(third_party/glad)
add_subdirectory(third_party/ImGuizmo)
add_subdirectory(third_party/json)
add_subdirectory(third_party/KHR)
add_subdirectory(third_party/STB)
add_subdirectory(third_party/objloader)

# source files and libs
add_executable(engine ${SOURCES})

# system dll
find_package(OpenGL REQUIRED)

target_link_libraries(engine PRIVATE glfw OpenGL::GL imgui glm::glm glad ImGuizmo json KHR objloader STB)
