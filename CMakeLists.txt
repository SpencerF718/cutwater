cmake_minimum_required(VERSION 3.10)

project(Cutwater C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wpedantic)

find_package(Curses REQUIRED)

add_executable(cutwater src/main.c)

target_link_libraries(cutwater ${CURSES_LIBRARIES})
