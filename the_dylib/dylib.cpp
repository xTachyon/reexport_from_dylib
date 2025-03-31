#include <common.hpp>

void staticlib_symbol_1();

__declspec(dllexport) void dylib_symbol_1() {
    // staticlib_symbol_1();
}
