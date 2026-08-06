CXX = g++
CXXFLAGS = -std=gnu++17 -Wall
BUILD = build

all: $(BUILD)/c_compiler

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/parser.tab.cpp $(BUILD)/parser.tab.h: src/parser/parser.y | $(BUILD)
	bison -d --defines=$(BUILD)/parser.tab.h -o $(BUILD)/parser.tab.cpp src/parser/parser.y

$(BUILD)/lex.yy.cpp: src/lexer/lexer.l $(BUILD)/parser.tab.h | $(BUILD)
	flex -o $(BUILD)/lex.yy.cpp src/lexer/lexer.l

$(BUILD)/c_compiler: $(BUILD)/parser.tab.cpp $(BUILD)/lex.yy.cpp \
                      src/ast/ast.cpp src/symbol_table/symbol_table.cpp \
                      src/semantic/semantic.cpp src/icg/icg.cpp \
                      src/optimizer/optimizer.cpp src/codegen/codegen.cpp \
                      src/main.cpp
	$(CXX) $(CXXFLAGS) -I$(BUILD) -o $@ $^

clean:
	rm -rf $(BUILD)

.PHONY: all clean