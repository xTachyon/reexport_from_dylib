Most of the information in this post are from experimentation. If you have better ways to do this, please tell me.

# The problem

I have a static lib (.lib, .a) that has some symbols inside it.

I have a dylib (.dll, .so, .dylib) where I want to reexport some of the symbols (usually functions) from the staticlib.

This has some usecases:
- maybe I'm trying to link zlib (or any other lib) statically into my core dylib, and have all the dylibs that depend on it also have it accesible, without duplicating the generated code
- maybe I want to logically split my lib into several smaller staticlibs, that still get exported through the same final dylib
- maybe I have a staticlib generated by Rust that's morally a dylib, but because I want to integrate it in a C++ project, it's easier to just have CMake generate the final dylib

