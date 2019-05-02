project(skalang)

include_directories(../../ska/ska_core/src/src)

include_directories("src")

link_directories(../../ska/lib)
link_directories(../lib)

file(GLOB_RECURSE SOURCES
    src/*.h
    src/*.cpp
)

string(REGEX REPLACE "[^;]*CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_library(skalang ${SOURCES})
find_package(Threads REQUIRED)
target_link_libraries(skalang ska_core skalog Threads::Threads)
target_include_directories(skalang PUBLIC ../external/skalog/src/src)
