
C_SOURCES=$(wildcard *.c) $(wildcard */*.c) $(wildcard */*/*.c)
C_OBJECTS=$(patsubst %.c,%.o,$(C_SOURCES))
A_SOURCES=$(wildcard *.asm) $(wildcard */*.asm) $(wildcard */*/*.asm)
A_OBJECTS=$(filter-out $(START),$(patsubst %.asm,%_a.o,$(A_SOURCES)))
OBJECTS+=$(C_OBJECTS) $(A_OBJECTS)
DEPENDS=$(patsubst %.c,%.d,$(C_SOURCES))
DEPENDS+= $(patsubst %.asm,%_a.d,$(A_SOURCES))

REL_PATH=$(shell realpath --relative-to $(ROOT_PROJECT_DIRECTORY) .)/
SRC_PATH=$(ROOT_PROJECT_DIRECTORY)$(REL_PATH)
OBJ_PATH=$(OBJ_DIRECTORY)$(REL_PATH)
OBJECTS:=$(addprefix $(OBJ_PATH),$(OBJECTS))
DEPENDS:=$(addprefix $(OBJ_PATH),$(DEPENDS))

INCLUDE=-I$(ROOT_PROJECT_DIRECTORY)src
LIBRARIES_FILE_NAMES=$(patsubst %,$(LIB_DIRECTORY)lib%.a,$(LIBRARIES))
override LDLIBS+=$(patsubst %,-l%,$(LIBRARIES))

.PHONY: all
all: $(OBJ_PATH) $(TARGET)
	@:

$(OBJ_PATH):
	$(AT)mkdir -p $@

.PHONY: clean
clean:
	$(AT)$(RM) $(TARGET) $(DEPENDS) $(OBJECTS)

$(OBJ_PATH)%.o: $(SRC_PATH)%.c Makefile
	$(AT)mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -MP -MF $(patsubst $(SRC_PATH)%.c,$(OBJ_PATH)%.d,$<) $(INCLUDE) -c $< -o $@

$(OBJ_PATH)%_a.o: $(SRC_PATH)%.asm Makefile
	$(AT)mkdir -p $(dir $@)
	$(AS) -felf $(CONSTANTS) $< -o $@ -MD $(patsubst $(SRC_PATH)%.asm,$(OBJ_PATH)%.d,$<)

-include $(DEPENDS)
