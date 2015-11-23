ROOT=.
APP=vype

SRC_FILES=main.cpp

LIBRARIES=frontend ir

include $(ROOT)/mak/config.mak

all: build

ir:
	@$(MAKE) -C ir

frontend: ir
	@$(MAKE) -C frontend

DEPENDS=ir frontend
include $(ROOT)/mak/app.mak

clean: main-clean
main-clean:
	@$(MAKE) -C frontend clean
	@$(MAKE) -C testing clean
	@$(MAKE) -C ir clean

test: all
	@$(MAKE) -C testing

.PHONY: ir frontend testing
