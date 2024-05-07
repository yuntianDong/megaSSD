
SUBDIRS := \
	.\
	
MKFILE_LIST	:=	$(foreach n,$(SUBDIRS) , $(wildcard Makefile.*))

all: GOAL=all
all: $(MKFILE_LIST)
	echo $(MKFILE_LIST)

clean: GOAL=clean
clean: $(MKFILE_LIST)
	rm -f *.so util*
	
$(MKFILE_LIST):
	$(MAKE) -f $@ $(GOAL)
	
.PHONY: all clean $(MKFILE_LIST)
	