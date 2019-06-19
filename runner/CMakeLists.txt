project(skalang_runner)

include_directories(${SKA_BASE_DIR})
include_directories(src)
include_directories(${SKALANG_SRC_DIR}/src)
include_directories(${SKALANG_STD_DIR}/src)

link_directories(../lib)

#Runner part
file(GLOB_RECURSE SOURCES
	src/*.h
	src/*.cpp
)

string(REGEX REPLACE "[^;]*CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_definitions(-DSKALANG_RUNNER_DIR="${SKALANG_RUNNER_DIR}")
add_executable(skalang_runner ${SOURCES})

find_package(Threads REQUIRED)
target_link_libraries(skalang_runner skalang_std skalang skalog skabase Threads::Threads)
target_include_directories(skalang_runner PUBLIC ../external/skabase/src/src PUBLIC ../external/skalog/src/src)
