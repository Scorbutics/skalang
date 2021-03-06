project(skalang_bytecode_runner)

include_directories(${SKA_BASE_DIR})
include_directories(src)
include_directories(${SKALANG_SRC_DIR}/src)
include_directories(${SKALANG_BYTECODE_GEN_DIR}/src)
include_directories(${SKALANG_BYTECODE_SER_DIR}/src)
include_directories(${SKALANG_BYTECODE_INT_DIR}/src)
include_directories(${SKALANG_STD_DIR}/src)

link_directories(../lib)

#Runner part
file(GLOB_RECURSE SOURCES
	src/*.h
	src/*.cpp
)

string(REGEX REPLACE "[^;]*CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_definitions(-DSKALANG_BYTECODE_RUN_DIR="${SKALANG_BYTECODE_RUN_DIR}")
generate_source_groups("${SOURCES}" "${SKALANG_BYTECODE_RUN_DIR}\\src")
add_executable(skalang_bytecode_runner ${SOURCES})

find_package(Threads REQUIRED)
set(LIBS skalang_bytecode_interpreter skalang_bytecode_generator skalang_bytecode_serializer skalang skalog skabase Threads::Threads)
IF(UNIX)
	set(LIBS ${LIBS} atomic)
ENDIF(UNIX)
target_link_libraries(skalang_bytecode_runner ${LIBS})
target_include_directories(skalang_bytecode_runner PUBLIC ../external/skabase/src/src PUBLIC ../external/skalog/src/src)
