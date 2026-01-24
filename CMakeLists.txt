cmake_minimum_required(VERSION 3.10)

project(Cutwater C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra -Wpedantic)

find_package(Curses REQUIRED)

set(SOURCES
    src/main.c
    src/buffer.c
)

set(TEST_SOURCES
    tests/test_buffer.c
    src/buffer.c
)

add_executable(cutwater ${SOURCES})
add_executable(run_tests ${TEST_SOURCES})

target_include_directories(cutwater PRIVATE include)
target_include_directories(run_tests PRIVATE include)

target_link_libraries(cutwater ${CURSES_LIBRARIES})
