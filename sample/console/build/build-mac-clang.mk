# @brief   ESM: Makefile for sample application (macOS clang)
# @author  eel3
# @date    2017-10-18

# ---------------------------------------------------------------------

PREFIX         := xcrun 
CC             := $(PREFIX)$(CC)

CFLAGS          =
LDFLAGS         =
LDLIBS         := -lc++

CCDEFS          =
OBJADD         :=
WARNADD        :=
USE_ASSERT     :=

# ---------------------------------------------------------------------

SDKROOT        := $(shell xcodebuild -version -sdk macosx | sed -n '/^Path: /s///p')

CPPFLAGS       := -isysroot "$(SDKROOT)"
TARGET_ARCH    := -mmacosx-version-min=10.11 -arch i386 -arch x86_64

# ---------------------------------------------------------------------

include ./build-common.mk
