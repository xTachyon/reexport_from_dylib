Most of the information in this post are learned from experimentation. If you have better ways to do this, please tell me.

# The problem

I have a static lib (.lib, .a) that has some symbols inside it.

I have a dylib (.dll, .so, .dylib) where I want to reexport some of the symbols (usually functions) from the staticlib.

This has some usecases:
- maybe I'm trying to link zlib (or any other lib) statically into my core dylib, and have all the dylibs that depend on it also have it accesible, without duplicating the generated code
- maybe I want to logically split my lib into several smaller staticlibs, that still get exported through the same final dylib
- maybe I have a staticlib generated by Rust that's morally a dylib, but because I want to integrate it in a C++ project, it's easier to just have CMake generate the final dylib

# How to test

On this repo, you'll find a test project that you can experiment with the diverse solutions.
The following directory/targets can be seen:
- `common`: contains a header with code that'll be used by all other targets, including functions declarations
- `the_staticlib`: a staticlib that contains functions like `staticlib_symbol_1`, `staticlib_symbol_2`
- `the_dylib`: the dylib that links the the_staticlib and wants to export both its own symbol `dylib_symbol_1`, and `staticlib_symbol_1`
- `the_bin`: a binary that links `the_dylib`, and where we expect the errors to happen

# Investigating on Windows

On Windows, to export a symbol from a dll, we need to mark it as [dllexport](https://learn.microsoft.com/en-us/cpp/cpp/dllexport-dllimport):
```cpp
__declspec(dllexport) void dylib_symbol_1();
__declspec(dllexport) void staticlib_symbol_1();
```

We can check the exports of a dll as following:
```
C:\repos\reexport_from_dylib\bin\Release>dumpbin /exports the_dylib.dll
Microsoft (R) COFF/PE Dumper Version 14.43.34808.0
Copyright (C) Microsoft Corporation.  All rights reserved.


Dump of file the_dylib.dll

File Type: DLL

  Section contains the following exports for the_dylib.dll

    00000000 characteristics
    FFFFFFFF time date stamp
        0.00 version
           1 ordinal base
           2 number of functions
           2 number of names

    ordinal hint RVA      name

          1    0 00001000 dylib_symbol_1
```

We can see the symbol from `the_dylib`, but not the once from `the_staticlib`. I really believed this would work from the start on Windows with `dllexport`, but I was wrong.

Why does this happen?

It seems like the linker has an algorithm as following:
- search all objs (.obj, .o) for symbols marked as `dllexport`, and include them
- include all the dependencies of the above symbols

This is the reason why the symbols from `the_staticlib` don't appear in the first place, as they're not ever used. The linker discards unused symbols (or does it?).

Calling `staticlib_symbol_1` from `dylib_symbol_1` marks the function used, and with no surprise, it does show as exported symbol.

```cpp
__declspec(dllexport) void dylib_symbol_1() {
    staticlib_symbol_1();
}
```

However, what is a surprise, is that both `staticlib_symbol_1` and `staticlib_symbol_2` show up as exports, even if we never used `staticlib_symbol_2`. My guess is that the linker works on object files, not on symbols directly, so even if just one symbol from that file is used, the whole file is pulled into the binary.

# Investigating on Linux

To make the code cross platform, we can pull the `dllexport` into a macro named `EXPORT`, that currently does nothing on non-Windows.

We compile the code, and.. it works?

We can check the symbols:
```
✗ nm ../bin/libthe_dylib.so 
0000000000004008 b completed.0
                 w __cxa_finalize@GLIBC_2.2.5
0000000000001040 t deregister_tm_clones
00000000000010b0 t __do_global_dtors_aux
0000000000003df0 d __do_global_dtors_aux_fini_array_entry
0000000000004000 d __dso_handle
0000000000001100 T dylib_symbol_1
0000000000003df8 d _DYNAMIC
0000000000001104 t _fini
00000000000010f0 t frame_dummy
0000000000003de8 d __frame_dummy_init_array_entry
0000000000002094 r __FRAME_END__
0000000000003fe8 d _GLOBAL_OFFSET_TABLE_
                 w __gmon_start__
0000000000002000 r __GNU_EH_FRAME_HDR
0000000000001000 t _init
                 w _ITM_deregisterTMCloneTable
                 w _ITM_registerTMCloneTable
0000000000001070 t register_tm_clones
0000000000004008 d __TMC_END__
```

.. What? Okay, the first column is probably the address, and the last is the name of the function, but what about the middle one? We can check the manual for `nm` to find out this!
```
...
           "T"
           "t" The symbol is in the text (code) section.

           "U" The symbol is undefined.

```

There are a lot of letters to check out, but the most relevant:
- t/T: this symbol is defined in this .so
- U: this symbol is from another binary that's supposed to be loaded at runtime

But what's the different between lowercase t and uppercase T?
```
If lowercase, the symbol is usually local; if uppercase, the symbol is global (external).
```
So the difference is that t is supposed to be a local symbol that exists, but it's not exported, while T is exported.

Now that we know this, we can see no symbol from `the_staticlib` are present, which is not a surprise at this point, but this is a surprise:
```
0000000000001100 T dylib_symbol_1
```
Not only `dylib_symbol_1` is defined, but it's also exported. Why is that?

It turns out that Unixes take the complete opposite to Windows. On Unixes, everything is public (exported) by default, while on Windows, everything is not exported.

# It works on Unixes!

But we don't like it. 