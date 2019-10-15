project(skalang_interpreter)

include_directories(${SKA_BASE_DIR})
include_directories(src)
include_directories(${SKALANG_SRC_DIR}/src)

link_directories(../lib)

#Runner part
file(GLOB_RECURSE SOURCES
	src/*.h
	src/*.cpp
)

string(REGEX REPLACE "[^;]*CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_definitions(-DSKALANG_INTERPRETER_DIR="${SKALANG_INTERPRETER_DIR}")
add_library(skalang_interpreter ${SOURCES})

find_package(Threads REQUIRED)
target_link_libraries(skalang_interpreter skalang skalog skabase Threads::Threads)
target_include_directories(skalang_interpreter PUBLIC ../external/skabase/src/src PUBLIC ../external/skalog/src/src)
