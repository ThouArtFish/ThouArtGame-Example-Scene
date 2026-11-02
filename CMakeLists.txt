cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Name your executable
set(PROJECT_NAME cool_example)

project(${PROJECT_NAME})

# Change where the .exe is located - asset path depends on where this is located
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/game)

# Path to source files
set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/game/src/)

include(FetchContent)
FetchContent_Declare(
    TAGEngine
    GIT_REPOSITORY https://github.com/ThouArtFish/ThouArtGame.git
    GIT_TAG        v0.2.0
)
FetchContent_MakeAvailable(TAGEngine)

file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS
    ${PROJECT_SOURCE_DIR}*.cpp
    ${PROJECT_SOURCE_DIR}*.c
)
add_executable(${PROJECT_NAME} ${GAME_SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE TAGEngine)