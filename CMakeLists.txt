cmake_minimum_required(VERSION 3.16)
project(cutwater C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)

include_directories(${GTK4_INCLUDE_DIRS})
link_directories(${GTK4_LIBRARY_DIRS})
add_definitions(${GTK4_CFLAGS_OTHER})

add_executable(cutwater src/gui/main.c)
target_link_libraries(cutwater ${GTK4_LIBRARIES})

