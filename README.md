ESM: Embedded State Machine
===========================

A simple framework for embedded C/C++ application (state machine like).

License
-------

zlib License.

Target environments
-------------------

Windows, Linux, macOS, etc.

ESM is written in ISO C99 (to be precise, ISO C90 with stdbool.h/stdint.h),
and so probably works fine on other OS.

In addition, C standard library functions used in ESM are stdbool.h/stddef.h/stdint.h.
So ESM probably works on bare metal environment.

Usage
-----

1.  Add all source files in [src/](src/) directory to your project.
2.  Implement machdep library.
    See [src/lib/esm_md.h](src/lib/esm_md.h),
    [src/machdep/sample/esm_md.c](src/machdep/sample/esm_md.c).
3.  Implement configuration header file.
    See [src/machdep/sample/esm_config.h](src/machdep/sample/esm_config.h).
4.  Use public API functions.
    See [src/include/esm.h](src/include/esm.h),
    [sample/console/](sample/console/).
