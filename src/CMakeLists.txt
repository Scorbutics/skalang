cmake_minimum_required(VERSION 3.9)
project(skalang)

include_directories(../../ska/ska_core/src)
include_directories(../external/skalog/src/src)

file(GLOB_RECURSE SOURCES
    src/*.h
    src/*.cpp
)

string(REGEX REPLACE "[^;]*CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_library(skalang ${SOURCES})


