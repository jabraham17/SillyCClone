TOPTARGETS= all clean

SUBDIRS= parser sema tools/ptviewer tools/astviewer

.PHONY: $(TOPTARGETS)

$(TOPTARGETS): $(SUBDIRS)

# dependencies
sema: parser
tools/ptviewer: parser
tools/astviewer: sema parser

.PHONY: $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

