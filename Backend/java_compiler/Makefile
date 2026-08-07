# Makefile for java_hub Java Compiler

CXX = g++
CXXFLAGS = -std=c++11 -Isrc/parser -Isrc/lexer -Isrc/ast -Isrc/semantic -Isrc/symbol_table -Isrc/icg -Isrc/optimizer -Isrc/executor
FLEX = win_flex
BISON = win_bison

SRCS = src/parser/parser.tab.cpp \
       src/lexer/lex.yy.cpp \
       src/lexer/tokens.cpp \
       src/ast/ast.cpp \
       src/semantic/semantic.cpp \
       src/symbol_table/symbol_table.cpp \
       src/icg/icg.cpp \
       src/optimizer/optimizer.cpp \
       src/executor/executor.cpp

TARGET = java_compiler.exe

all: $(TARGET)

src/parser/parser.tab.cpp: src/parser/parser.y
	$(BISON) -d -o src/parser/parser.tab.cpp src/parser/parser.y

src/lexer/lex.yy.cpp: src/lexer/lexer.l
	$(FLEX) -o src/lexer/lex.yy.cpp src/lexer/lexer.l

$(TARGET): src/parser/parser.tab.cpp src/lexer/lex.yy.cpp
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

clean:
	del /f /q src\parser\parser.tab.cpp src\parser\parser.tab.hpp src\lexer\lex.yy.cpp $(TARGET) 2>NUL || exit 0
