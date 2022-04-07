



C_SOURCES=$(wildcard *.c) $(wildcard */*.c) $(wildcard */*/*.c)
C_OBJECTS=$(patsubst %.c,%.o,$(C_SOURCES))
A_SOURCES=$(wildcard *.asm) $(wildcard */*.asm) $(wildcard */*/*.asm)
A_OBJECTS=$(filter-out $(START),$(patsubst %.asm,%_a.o,$(A_SOURCES)))
OBJECTS=$(C_OBJECTS) $(A_OBJECTS)
DEPENDS=$(patsubst %.c,%.d,$(C_SOURCES))
DEPENDS+= $(patsubst %.asm,%_a.d,$(A_SOURCES))

LIBRARIES=parser
OBJECTS+= $(patsubst %,$(LIB_DIRECTORY)%.o,$(LIBRARIES))

INCLUDE= -I$(ROOT_PROJECT_DIRECTORY)src

TARGET=$(BIN_DIRECTORY)ptviewer

.PHONY: all
all: $(TARGET)
	@:

.PHONY: clean
clean:
	@$(RM) $(TARGET) $(DEPENDS) $(OBJECTS)

$(TARGET): $(OBJECTS) Makefile
	@echo "LD $(@F)"
	@$(LD) $(OBJECTS) $(LDFLAGS) $(LDLIBS) -o $@

%.o: %.c Makefile
	@echo "CC $@"
	@$(CC) $(CFLAGS) $(DEPENFLAGS) $(INCLUDE) -c $< -o $@

%_a.o: %.asm Makefile
	@echo "AS $@"
	@$(AS) -felf $(CONSTANTS) $< -o $@ -MD $(patsubst %.asm,%.d,$<)

-include $(DEPENDS)
