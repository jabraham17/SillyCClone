TOPTARGETS= all clean

SUBDIRS= parser tools/ptviewer

.PHONY: $(TOPTARGETS)

$(TOPTARGETS): $(SUBDIRS)

# dependencies
tools/ptviewer: parser

.PHONY: $(SUBDIRS)
$(SUBDIRS):
	@echo "make --directory=$@ $(MAKECMDGOALS)"
	@$(MAKE) --directory=$@ $(MAKECMDGOALS)

