PROJECT=vype

ROOT=.
MODULE=

SRC_FILES=main.cpp

include $(ROOT)/mak/config.mak

all: build_all

build_all: frontend build
	$(CXX) -o $(PROJECT) $(OBJ_FILES) $(OBJ_DIR)/frontend/*.o

frontend:
	@$(MAKE) -C frontend

include $(ROOT)/mak/common.mak

clean: main-clean
main-clean:
	$(RM) $(PROJECT)
	@$(MAKE) -C frontend clean
	@$(MAKE) -C testing clean

test:
	$(MAKE) -C testing

.PHONY: frontend
