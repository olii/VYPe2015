ROOT=.
APP=vype

SRC_FILES=main.cpp

LIBRARIES=frontend ir backend

include $(ROOT)/mak/config.mak

all: build

ir:
	@$(MAKE) -C ir

frontend: ir
	@$(MAKE) -C frontend

backend: ir
	@$(MAKE) -C backend

DEPENDS=ir frontend backend
include $(ROOT)/mak/app.mak

clean: main-clean
main-clean:
	@$(MAKE) -C frontend clean
	@$(MAKE) -C testing clean
	@$(MAKE) -C ir clean
	@$(MAKE) -C backend clean

test: all
	@$(MAKE) -C testing

.PHONY: ir frontend testing
