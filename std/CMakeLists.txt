project(skalang_std)

include_directories(${SKA_BASE_DIR})
include_directories(src)
include_directories(${SKALANG_SRC_DIR}/src)
include_directories(${SKALANG_INTERPRETER_DIR}/src)

link_directories(../lib)

#Runner part
file(GLOB_RECURSE SOURCES
	src/*.h
	src/*.cpp
)

string(REGEX REPLACE "[^;]*CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_definitions(-DSKALANG_STD_DIR="${SKALANG_STD_DIR}")
add_library(skalang_std ${SOURCES})

find_package(Threads REQUIRED)
set(LIBS skalang skalang_interpreter skalog skabase Threads::Threads)
IF(UNIX)
    set(LIBS atomic ${LIBS})
ENDIF(UNIX)
target_link_libraries(skalang_std ${LIBS})
target_include_directories(skalang_std PUBLIC ../external/skabase/src/src PUBLIC ../external/skalog/src/src)
