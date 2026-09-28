cmake_minimum_required(VERSION 3.14)
project(engine)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# source files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/src/*.cpp)

# libraries
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_TESTS OFF CACHE BOOL "" FORCE)
set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
add_subdirectory(third_party/SDL)
add_subdirectory(third_party/imgui)
add_subdirectory(third_party/glm)
add_subdirectory(third_party/glad)
add_subdirectory(third_party/ImGuizmo)
add_subdirectory(third_party/json)
add_subdirectory(third_party/KHR)
add_subdirectory(third_party/STB)
add_subdirectory(third_party/assimp)
# source files and libs
add_executable(engine ${SOURCES})

# system dll
find_package(OpenGL REQUIRED)

target_link_libraries(engine PRIVATE OpenGL::GL imgui glm::glm glad ImGuizmo json KHR assimp STB "SDL3::SDL3-static")

target_compile_definitions(
	engine PRIVATE 
	ASSETS_DIR="../../Assets/"
	SRC_DIR="../../src/"
	ROOT_DIR="../../"
)