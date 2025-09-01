cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

# FetchContent block
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG master
)

FetchContent_MakeAvailable(stb)

# Instead of INTERFACE, create a dummy target that points to stb headers
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})