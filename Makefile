.PHONY: all clean tests lib apple1

all: apple1

SUBDIRS = tests src apple1

tests: lib
	$(MAKE) -C tests runtests

lib:
	$(MAKE) -C src all

apple1: lib
	$(MAKE) -C apple1 all

clean:
	for dir in $(SUBDIRS) ; do \
		$(MAKE) -C $$dir clean; \
	done
