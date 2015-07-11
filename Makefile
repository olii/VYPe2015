PROJECT=vype

ROOT=.
MODULE=

SRC_FILES=main.cpp

include $(ROOT)/mak/config.mak

RM=rm -rf
MKDIR=mkdir -p

all: init build_all

init:
	@$(MKDIR) objs/frontend

build_all: frontend build
	$(CXX) -o $(PROJECT) $(OBJ_FILES) objs/frontend/*.o

frontend:
	@$(MAKE) -C $@

clean:
	$(RM) $(PROJECT) objs

.PHONY: frontend

include $(ROOT)/mak/common.mak
