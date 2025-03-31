#include <common.hpp>

void staticlib_symbol_1();

EXPORT void dylib_symbol_1() {
    // staticlib_symbol_1();
}
