project(skalang_runner)

include_directories(${SKA_BASE_DIR})
include_directories(src)
include_directories(${SKALANG_SRC_DIR}/src)
include_directories(${SKALANG_STD_DIR}/src)
include_directories(${SKALANG_INTERPRETER_DIR}/src)

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

set(LIBS skalang_std skalang_interpreter skalang skalog skabase Threads::Threads)
IF(UNIX)
    set(LIBS atomic ${LIBS})
ENDIF(UNIX)
target_link_libraries(skalang_runner ${LIBS})
target_include_directories(skalang_runner PUBLIC ../external/skabase/src/src PUBLIC ../external/skalog/src/src)
