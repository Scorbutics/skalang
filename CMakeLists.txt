cmake_minimum_required(VERSION 3.9)
project(skalang)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(THREADS_PREFER_PTHREAD_FLAG ON)

add_subdirectory(external/doctest)
add_subdirectory(external/skalog/src)
add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(runner)

