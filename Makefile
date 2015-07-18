PROJECT=vype

ROOT=.
MODULE=

SRC_FILES=main.cpp

include $(ROOT)/mak/config.mak

all: init build_all

build_all: frontend build
	$(CXX) -o $(PROJECT) $(OBJ_FILES) $(OBJ_DIR)/frontend/*.o

frontend:
	@$(MAKE) -C frontend

.PHONY: frontend

include $(ROOT)/mak/common.mak

init: main-init
main-init:
	@$(MAKE) -C frontend init

clean: main-clean
main-clean:
	$(RM) $(PROJECT)
	@$(MAKE) -C frontend clean