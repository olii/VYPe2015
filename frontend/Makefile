ROOT=..
ARCHIVE=frontend
MODULE=frontend

LEXER=c_lexer
PARSER=c_parser

LEXER_FILE=$(LEXER).l
LEXER_SRC=$(LEXER).cpp
LEXER_OBJ=$(LEXER).o

PARSER_FILE=$(PARSER).y
PARSER_SRC=$(PARSER).cpp
PARSER_HDR=$(PARSER).h
PARSER_OBJ=$(PARSER).o

SRC_FILES= ast.cpp \
           context.cpp \
           name_mangler.cpp \
           symbol.cpp \
           symbol_table.cpp

CUSTOM_CLEAN_FILES= $(PARSER_SRC) \
					$(PARSER_HDR) \
					$(LEXER_SRC)

include $(ROOT)/mak/config.mak

OBJ_FILES+= $(OBJ_DIR)/$(LEXER_OBJ) $(OBJ_DIR)/$(PARSER_OBJ)

all: init lexer parser build

lexer: parser $(LEXER_FILE)
	flex -o $(LEXER_SRC) $(LEXER_FILE)
	$(CXX) $(CXXFLAGS) -o $(OBJ_DIR)/$(LEXER_OBJ) $(LEXER_SRC)

parser: $(PARSER_FILE)
	bison --defines=$(PARSER_HDR) -o $(PARSER_SRC) $(PARSER_FILE)
	$(CXX) $(CXXFLAGS) -o $(OBJ_DIR)/$(PARSER_OBJ) $(PARSER_SRC)

DEPENDS=parser lexer
include $(ROOT)/mak/archive.mak
