ROOT=../
MODULE=frontend

SRC_FILES= c_lexer.cpp \
		   c_parser.cpp

include $(ROOT)/mak/config.mak

LEXER=c_lexer
PARSER=c_parser

LEXER_FILE=$(LEXER).l
LEXER_SRC=$(LEXER).cpp
LEXER_OBJ=$(LEXER).o

PARSER_FILE=$(PARSER).y
PARSER_SRC=$(PARSER).cpp
PARSER_HDR=$(PARSER).h
PARSER_OBJ=$(PARSER).o

all: parser lexer build

lexer: $(LEXER_FILE)
	flex -o $(LEXER_SRC) $<

parser: $(PARSER_FILE)
	bison --defines=$(PARSER_HDR) -o $(PARSER_SRC) $<

include $(ROOT)/mak/common.mak
