TOPTARGETS=all clean

# TOOLS=$(filter-out tools/helper,$(patsubst %/,%,$(dir $(wildcard tools/*/.))))
SUBDIRS= parser sema ir tools/*

.PHONY: $(TOPTARGETS)

$(TOPTARGETS): $(SUBDIRS)

# dependencies
sema: parser
ir: sema parser
tools/ptviewer: parser
tools/astviewer: sema parser
tools/irdump: ir sema parser
tools/interp: ir sema parser

.PHONY: $(SUBDIRS)
$(SUBDIRS):
	@echo "MAKE -C $@ $(MAKECMDGOALS)"
	@$(MAKE) --no-print-directory -C $@ $(MAKECMDGOALS)
