
TOPTARGETS=all clean

SUBDIRS=src

MKFILE_PATH=$(abspath $(lastword $(MAKEFILE_LIST)))
MKFILE_DIR=$(dir $(MKFILE_PATH))
export ROOT_PROJECT_DIRECTORY=$(MKFILE_DIR)
export BIN_DIRECTORY=$(ROOT_PROJECT_DIRECTORY)bin/
export OBJ_DIRECTORY=$(BIN_DIRECTORY)obj/
export LIB_DIRECTORY=$(ROOT_PROJECT_DIRECTORY)lib/

export OS=$(shell uname)
ifeq ($(OS),Linux)
export CC=/usr/bin/gcc
export LD:=$(CC)
export YACC=/usr/bin/bison
export LEX=/usr/bin/flex
else ifeq ($(OS),Darwin)
export CC=/usr/local/opt/llvm/bin/clang
export LD:=$(CC)
export YACC=/usr/local/opt/bison/bin/bison
export LEX=/usr/local/opt/flex/bin/flex
else
$(error Unsupported build on $(OS))
endif
export AS=nasm


export VERBOSE=0
ifeq ($(VERBOSE),0)
export AT=@
else
export AT=
endif

define _generate_verbose_call
$1_0 = @echo "$1 $$$$@"; $($1)
$1_1 = $($1)
export $1 = $$($1_$(VERBOSE))
endef
define generate_verbose_call
$(eval $(call _generate_verbose_call,$1))
endef

map = $(foreach a,$(2),$(call $(1),$(a)))

$(call map,generate_verbose_call,CC LD YACC LEX AS)


test:
	# $(CC)
	# $(MAKE)
	# $(AT)

# clang cant find gcc_s, gcc doesnt compile things correctly
# clang with rtlib=compiler-rt has problems with relocation

ifeq ($(OS),Linux)
override LDFLAGS+= 
#-fuse-ld=/usr/bin/gold
endif

override CFLAGS+= -DOS_SPECIFIC=$(OS) -Wno-comment

export EXTENSION=c

ifeq ($(DEBUG),1)
override CFLAGS+= -DDEBUG=1 -g -O0
else
override CFLAGS+= -O3
endif

override CFLAGS+= -Wall -Wextra
override CFLAGS+= -masm=intel
override CFLAGS+= -std=c11
override ASFLAGS+=
override LDFLAGS+=
override LDFLAGS_FINAL+=
override INCLUDE+=
override YFLAGS+= -Wall
override LFLAGS+=

ifeq ($(OS),Darwin)
override LDFLAGS+= -L/usr/local/opt/llvm/lib
override LDFLAGS_FINAL+= -Wl,-rpath,/usr/local/opt/llvm/lib
override INCLUDE+= -I/usr/local/opt/llvm/include
endif


export CFLAGS
export ASFLAGS
export LDFLAGS
export LDFLAGS_FINAL
export INCLUDE
export YFLAGS
export LFLAGS

export DEPENFLAGS= -MMD -MP

.PHONY: $(TOPTARGETS)

all: $(BIN_DIRECTORY) $(LIB_DIRECTORY) $(OBJ_DIRECTORY)
	@:

$(BIN_DIRECTORY):
	$(AT)mkdir -p $@

$(LIB_DIRECTORY):
	$(AT)mkdir -p $@

$(OBJ_DIRECTORY):
	$(AT)mkdir -p $@

clean:
	$(AT)$(RM) -r $(BIN_DIRECTORY) $(LIB_DIRECTORY)

$(TOPTARGETS): $(SUBDIRS)

.PHONY: $(SUBDIRS)
$(SUBDIRS):
	@echo "make -C $@ $(MAKECMDGOALS)"
	@$(MAKE) --no-print-directory -C $@ $(MAKECMDGOALS)

