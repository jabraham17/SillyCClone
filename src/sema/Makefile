
C_SOURCES=$(wildcard *.c) $(wildcard */*.c) $(wildcard */*/*.c)
C_OBJECTS=$(patsubst %.c,%.o,$(C_SOURCES))
A_SOURCES=$(wildcard *.asm) $(wildcard */*.asm) $(wildcard */*/*.asm)
A_OBJECTS=$(filter-out $(START),$(patsubst %.asm,%_a.o,$(A_SOURCES)))
OBJECTS=$(C_OBJECTS) $(A_OBJECTS)
DEPENDS=$(patsubst %.c,%.d,$(C_SOURCES))
DEPENDS+= $(patsubst %.asm,%_a.d,$(A_SOURCES))

REL_PATH=$(shell realpath --relative-to $(ROOT_PROJECT_DIRECTORY) .)

INCLUDE= -I$(ROOT_PROJECT_DIRECTORY)src

TARGET=$(LIB_DIRECTORY)sema.o

.PHONY: all
all: $(TARGET)
	# $(REL_PATH)
	@:

.PHONY: clean
clean:
	$(AT)$(RM) $(TARGET) $(DEPENDS) $(OBJECTS)

$(TARGET): $(OBJECTS) Makefile
	$(LD) $(OBJECTS) $(LDFLAGS) $(LDLIBS) -r -o $@

%.o: %.c Makefile
	@$(CC) $(CFLAGS) $(DEPENFLAGS) $(INCLUDE) -c $< -o $@

%_a.o: %.asm Makefile
	$(AS) -felf $(CONSTANTS) $< -o $@ -MD $(patsubst %.asm,%.d,$<)

-include $(DEPENDS)
