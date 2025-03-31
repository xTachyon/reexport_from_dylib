
add_library(the_dylib SHARED dylib.cpp)
target_link_libraries(the_dylib PRIVATE the_staticlib)
target_link_libraries(the_dylib PRIVATE common)