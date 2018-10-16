cmake_minimum_required(VERSION 3.9)
project(skalang_src)

set(CMAKE_CXX_STANDARD 17)

set(SKA_DEBUG_DIR "Debug")
set(SKA_RELEASE_DIR "Release")

set(SKA_CORE_DIR ../../ska/ska_core)
set(SKA_LOG_DIR ../external/skalog)
include_directories(${SKA_LOG_DIR})
include_directories(${SKA_CORE_DIR})

file(GLOB_RECURSE SOURCES
	RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.h
    *.cpp
)

string(REGEX REPLACE "CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_library(skalang_src ${SOURCES})


