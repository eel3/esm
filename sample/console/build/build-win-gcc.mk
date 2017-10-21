# @brief   ESM: Makefile for sample application (Windows MinGW/TDM-GCC)
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
