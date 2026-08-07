CXX = g++
CXXFLAGS = -std=c++11 -Isrc/Parser -Isrc/Semantic -Isrc/IntermediateCode -Isrc/Optimizer -Isrc/TargetCode -Isrc/Lexical -Ibuild

BUILD_DIR = build
TARGET = $(BUILD_DIR)/cpp_compiler

SRCS = $(BUILD_DIR)/parser.tab.c \
       $(BUILD_DIR)/lex.yy.c \
       src/Parser/ast.cpp \
       src/Semantic/symbol_table.cpp \
       src/Semantic/semantic.cpp \
       src/IntermediateCode/icg.cpp \
       src/Optimizer/optimizer.cpp \
       src/TargetCode/virtual_machine.cpp 

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/parser.tab.c $(BUILD_DIR)/parser.tab.h: src/Parser/parser.y | $(BUILD_DIR)
	bison -d src/Parser/parser.y -o $(BUILD_DIR)/parser.tab.c

$(BUILD_DIR)/lex.yy.c: src/Lexical/lexer.l $(BUILD_DIR)/parser.tab.h
	flex -o $(BUILD_DIR)/lex.yy.c src/Lexical/lexer.l

$(TARGET): $(BUILD_DIR)/parser.tab.c $(BUILD_DIR)/lex.yy.c
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(BUILD_DIR)/parser.tab.c $(BUILD_DIR)/parser.tab.h $(BUILD_DIR)/lex.yy.c $(TARGET) $(TARGET).exe