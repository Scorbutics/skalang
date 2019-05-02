project(skalang_test)

include_directories(../../ska/ska_core/src/src)
include_directories(../src/src)

link_directories(../../ska/lib)
link_directories(../lib)

#Unit Tests part
file(GLOB_RECURSE SOURCES
	src/*.h
	src/*.cpp
)

string(REGEX REPLACE "[^;]*CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_executable(skalang_test ${SOURCES})

find_package(Threads REQUIRED)
target_link_libraries(skalang_test skalang ska_core skalog -Wl,--no-whole-archive Threads::Threads)
target_include_directories(skalang_test PUBLIC ../external/skalog/src/src)
target_include_directories(skalang_test PUBLIC ../external/doctest)
