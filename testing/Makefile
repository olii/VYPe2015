ROOT=../
MODULE=testing

UNIT_TESTS_BIN=$(ROOT)/vype-test

CUSTOM_CXXFLAGS= -I$(ROOT)/testing -Wno-missing-field-initializers -pthread
SRC_FILES= 	gtest/gtest-all.cpp \
			unit_tests_main.cpp \
			example_tests.cpp

all: build-unit-tests

build-unit-tests: init build
	$(CXX) $(OBJ_FILES) -o $(UNIT_TESTS_BIN) -lpthread

include $(ROOT)/mak/config.mak

include $(ROOT)/mak/common.mak

init: testing-init
testing-init:
	@mkdir -p $(OBJ_DIR)/gtest $(OBJ_DIR)/unit-tests

clean: testing-clean
testing-clean:
	$(RM) $(UNIT_TESTS_BIN)
