cmake_minimum_required(VERSION 3.9)
project(skalang)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(THREADS_PREFER_PTHREAD_FLAG ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(SKA_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/skabase/src/src)
set(SKA_LOG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/skalog/src/src)

set(SKALANG_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test)
set(SKALANG_RUNNER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/runner)
set(SKALANG_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

include_directories(${SKA_BASE_DIR})
include_directories(${SKA_LOG_DIR})
include_directories(src/src)

add_subdirectory(external/doctest)
add_subdirectory(external/skalog/src)
add_subdirectory(external/skabase/src)

add_subdirectory(${SKALANG_SRC_DIR})
add_subdirectory(${SKALANG_TEST_DIR})
add_subdirectory(${SKALANG_RUNNER_DIR})
