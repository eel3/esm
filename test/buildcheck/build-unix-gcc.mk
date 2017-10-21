# @brief   ESM: Makefile for checking the syntax (Unix GCC)
# @author  eel3
# @date    2017-10-18

# ---------------------------------------------------------------------

PREFIX         :=
CC             := $(PREFIX)$(CC)

CFLAGS          =
LDFLAGS         =
LDLIBS         :=

CCDEFS          =
OBJADD         :=
WARNADD        :=
USE_ASSERT     :=

# ---------------------------------------------------------------------

include ./build-common.mk
