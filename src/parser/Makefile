



C_SOURCES=$(wildcard *.c) $(wildcard */*.c) $(wildcard */*/*.c)
C_OBJECTS=$(patsubst %.c,%.o,$(C_SOURCES))
A_SOURCES=$(wildcard *.asm) $(wildcard */*.asm) $(wildcard */*/*.asm)
A_OBJECTS=$(filter-out $(START),$(patsubst %.asm,%_a.o,$(A_SOURCES)))
OBJECTS=$(C_OBJECTS) $(A_OBJECTS)
DEPENDS=$(patsubst %.c,%.d,$(C_SOURCES))
DEPENDS+= $(patsubst %.asm,%_a.d,$(A_SOURCES))

INCLUDE=

TARGET=$(LIB_DIRECTORY)parser.o

.PHONY: all
all: $(TARGET)
	@:

.PHONY: clean
clean:
	$(RM) $(TARGET) $(DEPENDS) $(OBJECTS) *.temp sapphire_parser.h sapphire.o

$(TARGET): sapphire.o $(OBJECTS) Makefile
	@echo "LD $(@F)"
	@$(LD) sapphire.o $(OBJECTS) $(LDFLAGS) $(LDLIBS) -r -o $@

%.o: %.c Makefile
	@echo "CC $@"
	@$(CC) $(CFLAGS) $(DEPENFLAGS) $(INCLUDE) -c $< -o $@

%_a.o: %.asm Makefile
	@echo "AS $@"
	@$(AS) -felf $(CONSTANTS) $< -o $@ -MD $(patsubst %.asm,%.d,$<)

sapphire.o: sapphire.yy sapphire.l Makefile
	@echo "BISON $@"
	@$(BISON) -Wall sapphire.yy -o __bison_sapphire.c.temp --header=sapphire_parser.h
	@echo "FLEX $@"
	@$(FLEX) -o __flex_sapphire.c.temp sapphire.l
	@echo "CC $@"
	@$(CC) -c -x c __bison_sapphire.c.temp -x none -o __bison_sapphire.o.temp
	@echo "CC $@"
	@$(CC) -c -x c __flex_sapphire.c.temp -x none -o __flex_sapphire.o.temp
	@echo "LD $@"
	@$(LD) __bison_sapphire.o.temp __flex_sapphire.o.temp -r -o $@

-include $(DEPENDS)
