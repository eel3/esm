# @brief   ESM: Makefile for sample application (Unix GCC)
# @author  eel3
# @date    2017-10-18

# ---------------------------------------------------------------------

PREFIX         :=
CC             := $(PREFIX)$(CC)

CFLAGS          =
LDFLAGS         = -pthread
LDLIBS         := -lstdc++

CCDEFS          =
OBJADD         :=
WARNADD        :=
USE_ASSERT     :=

# ---------------------------------------------------------------------

include ./build-common.mk
