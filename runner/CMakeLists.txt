project(skalang_runner)

include_directories(${SKA_BASE_DIR})
include_directories(../src/src)

link_directories(../lib)

#Runner part
file(GLOB_RECURSE SOURCES
	src/*.h
	src/*.cpp
)

string(REGEX REPLACE "[^;]*CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_executable(skalang_runner ${SOURCES})

find_package(Threads REQUIRED)
target_link_libraries(skalang_runner skalang -Wl,--allow-multiple-definition skabase skalog Threads::Threads)
target_include_directories(skalang_runner PUBLIC ../external/skabase/src/src PUBLIC ../external/skalog/src/src)
