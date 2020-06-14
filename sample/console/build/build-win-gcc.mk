# @brief   ESM: Makefile for sample application (Windows MinGW/TDM-GCC)
# @author  eel3
# @date    2020-05-31

# ---------------------------------------------------------------------

PREFIX         :=
CC             := $(PREFIX)gcc

CFLAGS          =
LDFLAGS         =
LDLIBS         := -lstdc++

CCDEFS          =
OBJADD         :=
WARNADD        :=
USE_ASSERT     :=

# ---------------------------------------------------------------------

include ./build-common.mk
