-include $(ROOT_PROJECT_DIRECTORY)src/sources.mk
$(TARGET): $(OBJECTS) Makefile
	$(AR) rcs $@ $(OBJECTS)
	$(RANLIB) $@
