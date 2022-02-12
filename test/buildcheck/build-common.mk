# @brief   ESM: Makefile for checking the syntax (Unix environment)
# @author  eel3
# @date    2022-02-10

# ---------------------------------------------------------------------

root-dir       := ../../src

include-dir    := $(root-dir)/include
lib-dir        := $(root-dir)/lib
machdep-dir    := $(root-dir)/machdep
md-sample-dir  := $(machdep-dir)/sample

#----------------------------------------------------------------------

VPATH          := $(lib-dir) $(md-sample-dir)

include-dirs   := $(addprefix -I , \
                  $(include-dir) \
                  $(VPATH))

object-files   := esm.o esm_md.o
depend-files   := $(subst .o,.d,$(object-files))

#----------------------------------------------------------------------

ifdef USE_ASSERT
CCDEFS     += -DDEBUG
else
CCDEFS     += -DNDEBUG
endif

CCDEFS     +=
OPTIM      ?= -O0
WARN       ?= -Wall -std=c99 -pedantic \
              -Wextra \
              -Wunused-result \
              -Wno-unused-function -Wbad-function-cast -Wcast-align \
                  -Wmissing-include-dirs -Wundef \
                  -Werror-implicit-function-declaration \
              # -Wno-long-long

CFLAGS     += $(OPTIM) $(WARN) $(WARNADD)
CPPFLAGS   += $(CCDEFS) $(include-dirs)
LDFLAGS    += $(OPTIM)

#----------------------------------------------------------------------

phony-targets  := all clean usage

.PHONY: $(phony-targets)

usage:
	# $(MAKE) -f build-<target-arch>.mk $(patsubst %,[%],$(phony-targets))

all: $(object-files)

clean:
	$(RM) $(object-files) $(depend-files)

#----------------------------------------------------------------------

ifneq "$(MAKECMDGOALS)" ""
ifneq "$(MAKECMDGOALS)" "clean"
ifneq "$(MAKECMDGOALS)" "usage"
  -include $(depend-files)
endif
endif
endif

# $(call make-depend,source-file,object-file,depend-file)
make-depend = $(CC) -MM -MF $3 -MP -MT $2 $(CFLAGS) $(CPPFLAGS) $1

%.o: %.c
	$(call make-depend,$<,$@,$(subst .o,.d,$@))
	$(COMPILE.c) $(OUTPUT_OPTION) $<
