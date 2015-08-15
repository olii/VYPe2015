ROOT=..
APP=vype-test
MODULE=testing

CUSTOM_CXXFLAGS= -I$(ROOT)/testing -Wno-missing-field-initializers -Wno-sign-compare -pthread
CUSTOM_LXXFLAGS= -lpthread

SRC_FILES= 	gtest/gtest-all.cpp \
			gtest/gtest-main.cpp \
			example_tests.cpp \
			frontend_tests.cpp

LIBRARIES=frontend

include $(ROOT)/mak/config.mak

all: build

include $(ROOT)/mak/app.mak

init: testing-init
testing-init:
	@mkdir -p $(OBJ_DIR)/gtest
