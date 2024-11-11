cmake_minimum_required(VERSION 3.27)
project(AiGames)

set(CMAKE_CXX_STANDARD 20)

add_definitions(-DPROJECT_DIR="${CMAKE_SOURCE_DIR}")

include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/heads/main.zip
)

FetchContent_MakeAvailable(googletest)

FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/5.0.zip
)

FetchContent_MakeAvailable(raylib)

FetchContent_Declare(
    nml
    URL https://github.com/nik-narayanan/niks-ml-library/archive/refs/tags/00.00.01.zip
)

FetchContent_MakeAvailable(nml)

function(add_test TARGET_NAME SOURCE_FILE)
    add_executable(${TARGET_NAME} ${SOURCE_FILE})
    target_link_libraries(${TARGET_NAME} gtest_main nml)
endfunction()

function(add_raylib TARGET_NAME SOURCE_FILE)
    add_executable(${TARGET_NAME} ${SOURCE_FILE})
    target_link_libraries(${TARGET_NAME} raylib nml)
endfunction()

add_raylib(AiGames main.cpp)

add_test(connect_four_tests games/connect_four/state_tests.cpp)