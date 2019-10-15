project(skalang_bytecode_interpreter)

include_directories(${SKA_BASE_DIR})
include_directories(src)
include_directories(${SKALANG_SRC_DIR}/src)
include_directories(${SKALANG_BYTECODE_GEN_DIR}/src)

link_directories(../lib)

#Runner part
file(GLOB_RECURSE SOURCES
	src/*.h
	src/*.cpp
)

string(REGEX REPLACE "[^;]*CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_definitions(-DSKALANG_BYTECODE_INT_DIR="${SKALANG_BYTECODE_INT_DIR}")
add_library(skalang_bytecode_interpreter ${SOURCES})

find_package(Threads REQUIRED)
target_link_libraries(skalang_bytecode_interpreter skalang skalog skabase skalang_bytecode_generator Threads::Threads)
target_include_directories(skalang_bytecode_interpreter PUBLIC ../external/skabase/src/src PUBLIC ../external/skalog/src/src)
