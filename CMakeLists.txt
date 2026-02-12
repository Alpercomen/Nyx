cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.30")
  cmake_policy(VERSION 3.5)
endif()

project(Nyx LANGUAGES CXX)

set(FETCHCONTENT_UPDATE_DISCONNECTED ON CACHE BOOL "Disable Update Fetch" FORCE)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")
include(SetupConfig)
include(HandleQt)

add_subdirectory("External")
add_subdirectory("Source/Application")

target_compile_definitions(
	Nyx 
	PRIVATE
	GLM_ENABLE_EXPERIMENTAL
    GLM_FORCE_SILENT_WARNINGS
    GLM_FORCE_ALIGNED_GENTYPES
	PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
)