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

doc:
	@$(MAKE) -C doc

DEPENDS=ir frontend backend
include $(ROOT)/mak/app.mak

clean: main-clean
main-clean:
	@$(MAKE) -C frontend clean
	@$(MAKE) -C ir clean
	@$(MAKE) -C backend clean

test: all
	@$(MAKE) -C testing

pack: doc clean
	tar -czvf xmilko01.tgz main.cpp Makefile frontend/* backend/* ir/* mak/* testing/* dokumentace.pdf rozsireni rozdeleni

.PHONY: ir frontend testing doc pack
