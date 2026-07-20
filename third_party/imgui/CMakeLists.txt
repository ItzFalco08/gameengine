cmake_minimum_required(VERSION 3.14)

project(imgui LANGUAGES CXX)

file(GLOB IMGUI_SOURCES CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/backends/*.cpp"
)

add_library(imgui STATIC ${IMGUI_SOURCES})

target_include_directories(imgui
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}
		${CMAKE_CURRENT_SOURCE_DIR}/backends
)

target_link_libraries(imgui PRIVATE glfw)
