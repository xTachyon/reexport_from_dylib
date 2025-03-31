
add_library(common INTERFACE common.hpp)
set_target_properties(common PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(common INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
