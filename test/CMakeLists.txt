cmake_minimum_required(VERSION 3.9)
project(skalang_test)

set(CMAKE_CXX_STANDARD 17)

set(SKA_DEBUG_DIR "Debug")
set(SKA_RELEASE_DIR "Release")

set(SKA_CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../ska/ska_core)
set(SKA_LANG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../src)
set(DOCTEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../ska/external/doctest)

include_directories(${SKA_CORE_DIR})
include_directories(${SKA_LANG_DIR})
include_directories(${DOCTEST_DIR})

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../ska/${SKA_DEBUG_DIR})

file(GLOB_RECURSE SOURCES
	RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
	*.h
	*.cpp
	${SKA_LANG_DIR}/*.cpp
	${SKA_LANG_DIR}/*.h
)

string(REGEX REPLACE "CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")
string(REGEX REPLACE "skalang.cpp" "" SOURCES "${SOURCES}")
#string(REGEX REPLACE "\\W*/CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_executable(skalang_test ${SOURCES})
set_target_properties(skalang_test PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${SKA_DEBUG_DIR}
	      	ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/${SKA_DEBUG_DIR}
        	ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/${SKA_RELEASE_DIR})

target_link_libraries(skalang_test ska_core)
