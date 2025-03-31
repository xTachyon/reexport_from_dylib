cmake_minimum_required(VERSION 3.29)

project(reexport_from_dylib)

add_subdirectory(the_staticlib)
add_subdirectory(the_dylib)
add_subdirectory(the_bin)