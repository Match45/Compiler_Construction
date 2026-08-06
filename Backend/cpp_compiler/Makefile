CXX = g++
CXXFLAGS = -std=c++11 -Isrc/Parser -Isrc/Semantic -Isrc/IntermediateCode -Isrc/Optimizer -Isrc/TargetCode -Isrc/Lexical

TARGET = cpp_compiler

SRCS = src/Parser/parser.tab.c \
       src/Lexical/lex.yy.c \
       src/Parser/ast.cpp \
       src/Semantic/symbol_table.cpp \
       src/Semantic/semantic.cpp \
       src/IntermediateCode/icg.cpp \
       src/Optimizer/optimizer.cpp \
       src/TargetCode/virtual_machine.cpp

all: $(TARGET)

src/Parser/parser.tab.c src/Parser/parser.tab.h: src/Parser/parser.y
	bison -d src/Parser/parser.y -o src/Parser/parser.tab.c

src/Lexical/lex.yy.c: src/Lexical/lexer.l src/Parser/parser.tab.h
	flex -o src/Lexical/lex.yy.c src/Lexical/lexer.l

$(TARGET): src/Parser/parser.tab.c src/Lexical/lex.yy.c
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f src/Parser/parser.tab.c src/Parser/parser.tab.h src/Lexical/lex.yy.c $(TARGET) $(TARGET).exe