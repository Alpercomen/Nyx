cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project("Nyx")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(FETCHCONTENT_UPDATE_DISCONNECTED ON CACHE BOOL "Disable Update Fetch" FORCE)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")

include(SetupConfig)
SetupConfig()

add_subdirectory("External")
add_subdirectory("Source/Application")