
add_executable(the_bin main.cpp)
target_link_libraries(the_bin PRIVATE the_dylib)
target_link_libraries(the_bin PRIVATE common)