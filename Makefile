
TOPTARGETS=all clean

SUBDIRS=src

MKFILE_PATH=$(abspath $(lastword $(MAKEFILE_LIST)))
MKFILE_DIR=$(dir $(MKFILE_PATH))
export ROOT_PROJECT_DIRECTORY=$(MKFILE_DIR)
export BIN_DIRECTORY=$(ROOT_PROJECT_DIRECTORY)bin/
export LIB_DIRECTORY=$(ROOT_PROJECT_DIRECTORY)lib/

export OS=$(shell uname)
ifeq ($(OS),Linux)
export CC=/usr/bin/gcc
export BISON=/usr/bin/bison
export FLEX=/usr/bin/flex
else
export CC=/usr/local/opt/llvm/bin/clang
export BISON=/usr/local/opt/bison/bin/bison
export FLEX=/usr/local/opt/flex/bin/flex
endif
export AS=nasm
export LD=$(CC)

export EXTENSION=c

ifeq ($(DEBUG),1)
override CFLAGS+= -DDEBUG=1 -g -O0
else
override CFLAGS+= -O3
endif

override CFLAGS+= -Wall -Wextra
override CFLAGS+= -masm=intel
override CFLAGS+= -std=gnu11
override ASFLAGS+=

override LDFLAGS+=
override LDFLAGS+=
override INCLUDE+=


export CFLAGS
export ASFLAGS
export LDFLAGS
export INCLUDE

export DEPENFLAGS= -MMD -MP

.PHONY: $(TOPTARGETS)

all: $(BIN_DIRECTORY) $(LIB_DIRECTORY)
	@:

$(BIN_DIRECTORY):
	@mkdir -p $@

$(LIB_DIRECTORY):
	@mkdir -p $@

clean:
	$(RM) -r $(BIN_DIRECTORY) $(LIB_DIRECTORY)

$(TOPTARGETS): $(SUBDIRS)

.PHONY: $(SUBDIRS)
$(SUBDIRS):
	@echo "make --directory=$@ $(MAKECMDGOALS)"
	@$(MAKE) --directory=$@ $(MAKECMDGOALS)

