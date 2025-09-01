cmake_minimum_required(VERSION 3.16)
project(cutwater C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find GTK4
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)

# Include headers
include_directories(
    ${GTK4_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/include
)

link_directories(${GTK4_LIBRARY_DIRS})
add_definitions(${GTK4_CFLAGS_OTHER})

# Source files
set(SOURCES
    src/gui/main.c
    src/file_io/file_io.c
)

# Create executable
add_executable(cutwater ${SOURCES})

# Link GTK4 libraries
target_link_libraries(cutwater ${GTK4_LIBRARIES})

