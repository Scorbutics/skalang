project(skalang_bytecode_runner)

include_directories(${SKA_BASE_DIR})
include_directories(src)
include_directories(${SKALANG_SRC_DIR}/src)
include_directories(${SKALANG_STD_DIR}/src)
include_directories(${SKALANG_INTERPRETER_DIR}/src)
include_directories(${SKALANG_BYTECODE_GEN_DIR}/src)
include_directories(${SKALANG_BYTECODE_INT_DIR}/src)

link_directories(../lib)

#Runner part
file(GLOB_RECURSE SOURCES
	src/*.h
	src/*.cpp
)

string(REGEX REPLACE "[^;]*CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_definitions(-DSKALANG_BYTECODE_RUN_DIR="${SKALANG_BYTECODE_RUN_DIR}")
add_executable(skalang_bytecode_runner ${SOURCES})

find_package(Threads REQUIRED)
target_link_libraries(skalang_bytecode_runner atomic skalang_bytecode_interpreter skalang_bytecode_generator skalang_std skalang_interpreter skalang skalog skabase Threads::Threads)
target_include_directories(skalang_bytecode_runner PUBLIC ../external/skabase/src/src PUBLIC ../external/skalog/src/src)
