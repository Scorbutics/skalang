cmake_minimum_required(VERSION 3.9)
project(skalang)

set(CMAKE_CXX_STANDARD 17)
include_directories(../../ska/ska_core/src)
include_directories(../external/skalog/src/src)

include_directories("src")

link_directories(../../ska/ska_core/lib)
link_directories(../lib)

file(GLOB_RECURSE SOURCES
    src/*.h
    src/*.cpp
)

string(REGEX REPLACE "[^;]*CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
add_library(skalang ${SOURCES})
find_package(Threads REQUIRED)
target_link_libraries(skalang ska_core skalog Threads::Threads)

