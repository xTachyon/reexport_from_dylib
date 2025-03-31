cmake_minimum_required(VERSION 3.29)

project(reexport_from_dylib)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/")

add_subdirectory(common)
add_subdirectory(the_staticlib)
add_subdirectory(the_dylib)
add_subdirectory(the_bin)