# @brief   ESM: Makefile for checking the syntax (NMAKE and Microsoft C/C++ Optimizing Compiler)
# @author  eel3
# @date    2017-10-18

# ---------------------------------------------------------------------

root_dir        = ..\..\src

include_dir     = $(root_dir)\include
lib_dir         = $(root_dir)\lib
machdep_dir     = $(root_dir)\machdep
md_sample_dir   = $(machdep_dir)\sample

#----------------------------------------------------------------------

vpath           = $(lib_dir) $(md_sample_dir)

include_dirs    = $(include_dir)\
                  $(vpath)

object_files    = esm.obj esm_md.obj

# ----------------------------------------------------------

#ccdefs = /MTd /Zi /D WIN32;_DEBUG;_CONSOLE;_MBCS;WINVER=0x0601;_WIN32_WINNT=0x0601;_CRT_SECURE_NO_WARNINGS;_WINDOWS;DEBUG
ccdefs  = /MT /D WIN32;NDEBUG;_CONSOLE;_MBCS;WINVER=0x0601;_WIN32_WINNT=0x0601;_CRT_SECURE_NO_WARNINGS;_WINDOWS

#----------------------------------------------------------------------

CFLAGS  = /nologo /GL /GS /RTCs /RTCu /W4 $(ccdefs:;= /D ) /I $(include_dirs: = /I )

#----------------------------------------------------------------------

phony_targets   = all clean usage

usage: FORCE
	:: $(MAKE) /f build-win-vc.mak [$(phony_targets: =] [)]

all: $(object_files)

clean: FORCE
	del /F $(object_files) 2>nul

FORCE:

# ----------------------------------------------------------

{$(lib_dir)}.c.obj::
	$(CC) $(CFLAGS) /c $<
{$(md_sample_dir)}.c.obj::
	$(CC) $(CFLAGS) /c $<
