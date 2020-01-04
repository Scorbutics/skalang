project(skalang)

include_directories(src)

link_directories(../lib)

file(GLOB_RECURSE SOURCES
	src/*.h
	src/*.cpp
)

string(REGEX REPLACE "[^;]*CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_definitions(-DSKALANG_SRC_DIR="${SKALANG_SRC_DIR}")
add_library(skalang ${SOURCES})
find_package(Threads REQUIRED)
target_link_libraries(skalang skalog skabase Threads::Threads)
target_include_directories(skalang PUBLIC ../external/skabase/src/src PUBLIC ../external/skalog/src/src)
