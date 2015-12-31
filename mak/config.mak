CXX=g++-5.1
CXXFLAGS=-static -std=c++11 -Wall -Wextra -Wno-unused-function -c -I$(ROOT) $(CUSTOM_CXXFLAGS)
LXXFLAGS= $(CUSTOM_LXXFLAGS)
MAKEFLAGS=-j4

ifndef MODULE
	OBJ_DIR=$(ROOT)/objs
else
	OBJ_DIR=$(ROOT)/objs/$(MODULE)
endif
LIBS_DIR=$(ROOT)/libs

OBJ_FILES=$(patsubst %.cpp, $(OBJ_DIR)/%.o, $(SRC_FILES))
LIBRARIES_PATHS=$(patsubst %, $(LIBS_DIR)/lib%.a, $(LIBRARIES))

CLEAN_FILES= $(OBJ_FILES) $(CUSTOM_CLEAN_FILES)
ifdef APP
	CLEAN_FILES+= $(ROOT)/$(APP)
endif
ifdef ARCHIVE
	CLEAN_FILES+= $(LIBS_DIR)/lib$(ARCHIVE).a
endif

RM=rm -rf
