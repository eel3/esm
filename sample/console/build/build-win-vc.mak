# @brief   ESM: Makefile for sample application (NMAKE and Microsoft C/C++ Optimizing Compiler)
# @author  eel3
# @date    2017-10-18

# ---------------------------------------------------------------------

root_dir        = ..\..\..

src_dir         = $(root_dir)\src
include_dir     = $(src_dir)\include
lib_dir         = $(src_dir)\lib
machdep_dir     = $(src_dir)\machdep
md_sample_dir   = $(machdep_dir)\sample

app_dir         = ..

#----------------------------------------------------------------------

vpath           = $(lib_dir) $(app_dir)

include_dirs    = $(include_dir)\
                  $(md_sample_dir)\
                  $(vpath)

object_files    = esm.obj\
                  esm_md.obj\
                  main.obj\
                  handler_common.obj\
                  handler_state_1.obj\
                  handler_state_2.obj\
                  handler_state_3.obj

target_name     = console.exe

# ----------------------------------------------------------

#ccdefs = /MTd /Zi /D WIN32;_DEBUG;_CONSOLE;_MBCS;WINVER=0x0601;_WIN32_WINNT=0x0601;_CRT_SECURE_NO_WARNINGS;_WINDOWS;DEBUG
ccdefs  = /MT /D WIN32;NDEBUG;_CONSOLE;_MBCS;WINVER=0x0601;_WIN32_WINNT=0x0601;_CRT_SECURE_NO_WARNINGS;_WINDOWS

#----------------------------------------------------------------------

CFLAGS      = /nologo /GL /GS /RTCs /RTCu /W4 $(ccdefs:;= /D ) /I $(include_dirs: = /I )
CXXFLAGS    = /nologo /EHsc /GL /GS /RTCs /RTCu /W4 $(ccdefs:;= /D ) /I $(include_dirs: = /I )

#----------------------------------------------------------------------

phony_targets   = all clean usage

usage: FORCE
	:: $(MAKE) /f build-win-vc.mak [$(phony_targets: =] [)]

all: $(target_name)

$(target_name): $(object_files)
	link.exe /LTCG /OUT:$(target_name) /SUBSYSTEM:CONSOLE $(object_files) user32.lib

clean: FORCE
	del /F $(target_name) $(object_files) 2>nul

FORCE:

# ----------------------------------------------------------

{$(lib_dir)}.c.obj::
	$(CC) $(CFLAGS) /c $<
{$(app_dir)}.c.obj::
	$(CC) $(CFLAGS) /c $<
{$(app_dir)}.cpp.obj::
	$(CXX) $(CXXFLAGS) /c $<
