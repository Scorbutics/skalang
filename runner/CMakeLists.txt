project(skalang_runner)

include_directories(../../ska/ska_core/src/src)
include_directories(../src/src)

link_directories(../../ska/lib)
link_directories(../lib)

#Runner part
file(GLOB_RECURSE SOURCES
	src/*.h
	src/*.cpp
)

string(REGEX REPLACE "[^;]*CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_executable(skalang_runner ${SOURCES})

find_package(Threads REQUIRED)
target_link_libraries(skalang_runner skalang -Wl,--allow-multiple-definition ska_core skalog Threads::Threads)
target_include_directories(skalang_runner PUBLIC ../external/skalog/src/src)
