ROOT=.
APP=vype

SRC_FILES=main.cpp

LIBRARIES=frontend ir

include $(ROOT)/mak/config.mak

all: ir frontend build

frontend:
	@$(MAKE) -C frontend

ir:
	@$(MAKE) -C ir

include $(ROOT)/mak/app.mak

clean: main-clean
main-clean:
	@$(MAKE) -C frontend clean
	@$(MAKE) -C testing clean
	@$(MAKE) -C ir clean

test: all
	@$(MAKE) -C testing

.PHONY: ir frontend testing
