cmake_minimum_required(VERSION 3.20)

# Name your executable
set(PROJECT_NAME cool_example)

project(${PROJECT_NAME})
include(FetchContent)

FetchContent_Declare(
    TAGEngine
    GIT_REPOSITORY https://github.com/ThouArtFish/ThouArtGame.git
    GIT_TAG        v0.1.0
)
FetchContent_MakeAvailable(TAGEngine)

# Change where the .exe is located, asset path depends on where this is located
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/game/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/game/start.cpp
)
add_executable(${PROJECT_NAME} ${GAME_SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE TAGEngine)