-include $(ROOT_PROJECT_DIRECTORY)src/sources.mk

$(TARGET): $(OBJECTS) Makefile
	$(LD) $(OBJECTS) $(LDFLAGS) $(LDFLAGS_FINAL) -L$(LIB_DIRECTORY) $(LDLIBS) -o $@
