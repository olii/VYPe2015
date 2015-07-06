PROJECT=vype

LEXER=c_lexer
PARSER=c_parser

LEXER_FILE=$(LEXER).l
LEXER_SRC=$(LEXER).cpp
LEXER_OBJ=$(LEXER).o

PARSER_FILE=$(PARSER).y
PARSER_SRC=$(PARSER).cpp
PARSER_HDR=$(PARSER).h
PARSER_OBJ=$(PARSER).o

CXXFLAGS=-std=c++11 -Wall -Wextra -g3

SRCS=$(filter-out $(LEXER_SRC) $(PARSER_SRC), $(wildcard *.cpp))
OBJS=$(patsubst %.cpp, %.o, $(SRCS))
RM=rm -rf

all: parser lexer build

build: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(PROJECT) $^ $(LEXER_OBJ) $(PARSER_OBJ)

lexer: $(LEXER_FILE)
	flex -o $(LEXER_SRC) $<
	$(CXX) $(CXXFLAGS) -c -o $(LEXER_OBJ) $(LEXER_SRC)

parser: $(PARSER_FILE)
	bison --defines=$(PARSER_HDR) -o $(PARSER_SRC) $<
	$(CXX) $(CXXFLAGS) -c -o $(PARSER_OBJ) $(PARSER_SRC)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	$(RM) $(PROJECT) $(OBJS) $(LEXER_SRC) $(PARSER_SRC) $(PARSER_HDR)
