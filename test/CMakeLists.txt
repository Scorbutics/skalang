project(skalang_test)

include_directories(${SKALANG_SRC_DIR}/src)
include_directories(${SKALANG_STD_DIR}/src)

link_directories(../lib)

#Unit Tests part
file(GLOB_RECURSE SOURCES
	src/*.h
	src/*.cpp
)

string(REGEX REPLACE "[^;]*CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_definitions(-DSKALANG_TEST_DIR="${SKALANG_TEST_DIR}")
add_executable(skalang_test ${SOURCES})

find_package(Threads REQUIRED)
target_link_libraries(skalang_test skalang skalang_std skalog skabase Threads::Threads)
target_include_directories(skalang_test PUBLIC ../external/skalog/src/src)
target_include_directories(skalang_test PUBLIC ../external/skabase/src/src)
target_include_directories(skalang_test PUBLIC ../external/doctest)
