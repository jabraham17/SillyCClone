TARGET=$(LIB_DIRECTORY)libparser.a
OBJECTS+= sapphire.yy.o sapphire.l.o
-include $(ROOT_PROJECT_DIRECTORY)src/library.mk


$(OBJ_PATH)sapphire.yy.o: $(OBJ_PATH)sapphire.yy.c Makefile
	$(CC) -c -x c $< -x none -o $@ -I$(SRC_PATH)

$(OBJ_PATH)sapphire.l.o: $(OBJ_PATH)sapphire.l.c $(OBJ_PATH)sapphire.yy.h Makefile
	$(CC) -c -x c $< -x none -o $@ -I$(SRC_PATH) -I$(OBJ_PATH)

$(OBJ_PATH)sapphire.yy.c $(OBJ_PATH)sapphire.yy.h: $(SRC_PATH)sapphire.yy Makefile
ifeq ($(OS),Linux)
	$(YACC) $(YFLAGS) $< -o $(OBJ_PATH)sapphire.yy.c --defines=$(OBJ_PATH)sapphire.yy.h
else ifeq ($(OS),Darwin)
	$(YACC) $(YFLAGS) $< -o $(OBJ_PATH)sapphire.yy.c --header=$(OBJ_PATH)sapphire.yy.h
else
	$(warning Build will likely fail as builds on $(OS) are not fully supported)
	$(YACC) $(YFLAGS) $< -o $(OBJ_PATH)sapphire.yy.c -d
endif

$(OBJ_PATH)sapphire.l.c: $(SRC_PATH)sapphire.l $(OBJ_PATH)sapphire.yy.h Makefile
	$(LEX) $(LFLAGS) -o $@ $<

