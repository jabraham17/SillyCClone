TARGET=$(LIB_DIRECTORY)libparser.a
OBJECTS+= sapphire.o
-include $(ROOT_PROJECT_DIRECTORY)src/subdir.mk

$(OBJ_PATH)sapphire.o: $(SRC_PATH)sapphire.yy sapphire.l Makefile
ifeq ($(OS),Linux)
	$(YACC) $(YFLAGS) sapphire.yy -o __bison_sapphire.c.temp --defines=sapphire_parser.h
else ifeq ($(OS),Darwin)
	$(YACC) $(YFLAGS) sapphire.yy -o __bison_sapphire.c.temp --header=sapphire_parser.h
else
	$(warning Build will likely fail as builds on $(OS) are not fully supported)
	$(YACC) $(YFLAGS) sapphire.yy -o __bison_sapphire.c.temp -d
endif
	$(LEX) $(LFLAGS) -o __flex_sapphire.c.temp sapphire.l
	$(CC) -c -x c __bison_sapphire.c.temp -x none -o __bison_sapphire.o.temp
	$(CC) -c -x c __flex_sapphire.c.temp -x none -o __flex_sapphire.o.temp
	$(LD) __bison_sapphire.o.temp __flex_sapphire.o.temp $(LDFLAGS) $(LDLIBS) -r -o $@

