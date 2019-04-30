cmake_minimum_required(VERSION 3.9)
project(skalang_test)

set(CMAKE_CXX_STANDARD 17)
include_directories(../../ska/ska_core/src)
include_directories(../external/doctest)
include_directories(../external/skalog/src/src)
include_directories(../src/src)

link_directories(../../ska/ska_core/lib)
link_directories(../lib)
link_directories(../bin)

#Unit Tests part
file(GLOB_RECURSE SOURCES
	src/*.h
	src/*.cpp
)

string(REGEX REPLACE "[^;]*CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_executable(skalang_test ${SOURCES})

find_package(Threads REQUIRED)
target_link_libraries(skalang_test skalang ska_core skalog Threads::Threads)
