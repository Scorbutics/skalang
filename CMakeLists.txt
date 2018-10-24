cmake_minimum_required(VERSION 3.9)
project(skalang)

set(CMAKE_CXX_STANDARD 17)
add_subdirectory(external/doctest)
add_subdirectory(external/skalog)
add_subdirectory(src)

set(SKA_DEBUG_DIR "Debug")
set(SKA_RELEASE_DIR "Release")
set(SKA_CORE_DIR ../ska/ska_core)

include_directories(${SKA_CORE_DIR})
include_directories(external/doctest)
include_directories(external/skalog)
include_directories(src)
link_directories(../ska/${SKA_DEBUG_DIR})
link_directories(src/${SKA_DEBUG_DIR})
link_directories(external/skalog/${SKA_DEBUG_DIR})

#Unit Tests part
file(GLOB_RECURSE SOURCES
	external/*.h
	external/*.cpp
	test/*.h
	test/*.cpp
)

string(REGEX REPLACE "[^;]*CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_executable(skalang ${SOURCES})

set(THREADS_PREFER_PTHREAD_FLAG ON)
set_target_properties(skalang PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY ${SKA_DEBUG_DIR}
	      	ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${SKA_DEBUG_DIR}
        	ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${SKA_RELEASE_DIR})

find_package(Threads REQUIRED)
target_link_libraries(skalang skalang_src ska_core skalog Threads::Threads)
