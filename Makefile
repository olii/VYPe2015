ROOT=.
APP=vype

SRC_FILES=main.cpp

LIBRARIES=frontend

include $(ROOT)/mak/config.mak

all: frontend build

frontend:
	@$(MAKE) -C frontend

include $(ROOT)/mak/app.mak

clean: main-clean
main-clean:
	@$(MAKE) -C frontend clean
	@$(MAKE) -C testing clean

test: all
	@$(MAKE) -C testing

.PHONY: frontend testing
