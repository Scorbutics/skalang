cmake_minimum_required(VERSION 3.9)
project(skalang)

set(CMAKE_CXX_STANDARD 17)

set(SKA_DEBUG_DIR "Debug")
set(SKA_RELEASE_DIR "Release")

set(SKA_CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../ska/ska_core)

include_directories(${SKA_CORE_DIR})
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../ska/${SKA_DEBUG_DIR})

file(GLOB_RECURSE SOURCES
	RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.h
    *.cpp
)

string(REGEX REPLACE "CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_executable(skalang ${SOURCES})
set_target_properties(skalang PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${SKA_DEBUG_DIR}
	      	ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/${SKA_DEBUG_DIR}
        	ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/${SKA_RELEASE_DIR})

target_link_libraries(skalang ska_core)