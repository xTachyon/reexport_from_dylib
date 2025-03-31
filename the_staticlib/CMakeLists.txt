
add_library(the_staticlib STATIC staticlib.cpp)
target_link_libraries(the_staticlib PRIVATE common)