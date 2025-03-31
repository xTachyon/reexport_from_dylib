#pragma once

#ifdef _MSC_VER
#    define EXPORT __declspec(dllexport)
#else
#    define EXPORT
#endif

extern "C" {
EXPORT void staticlib_symbol_1();
EXPORT void staticlib_symbol_2();

EXPORT void dylib_symbol_1();
}