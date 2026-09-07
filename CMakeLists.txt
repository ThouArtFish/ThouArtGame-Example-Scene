cmake_minimum_required(VERSION 3.20)
include(FetchContent)
FetchContent_Declare(
    TAGEngine
    GIT_REPOSITORY https://github.com/ThouArtFish/ThouArtGame.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(TAGEngine)

add_executable(mygame game/src/start.cpp)
target_link_libraries(mygame PRIVATE TAGEngine)