# @brief   ESM: Makefile for checking the syntax (Windows MinGW/TDM-GCC)
# @author  eel3
# @date    2017-10-18

# ---------------------------------------------------------------------

PREFIX         :=
CC             := $(PREFIX)gcc

CFLAGS          =
LDFLAGS         =
LDLIBS         :=

CCDEFS          =
OBJADD         :=
WARNADD        :=
USE_ASSERT     :=

# ---------------------------------------------------------------------

include ./build-common.mk
