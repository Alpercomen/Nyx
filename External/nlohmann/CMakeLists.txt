cmake_minimum_required(VERSION 3.20)

include(FetchContent)

# Make URL variables
if(NOT DEFINED NLOHMANN_JSON_URL)
    set(NLOHMANN_JSON_URL https://github.com/nlohmann/json.git)
endif()

# nlohmann/json Library
FetchContent_Declare(
        nlohmann_json
        EXCLUDE_FROM_ALL
        GIT_REPOSITORY ${NLOHMANN_JSON_URL}
        GIT_TAG v3.11.3
        GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(nlohmann_json)