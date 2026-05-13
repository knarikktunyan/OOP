# Lexer + tokenizer + parser + sym + vm; outputs: compiler, vm
CXX      ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Iinclude

SRC := src/lexer.cpp src/tokenizer.cpp src/parser.cpp src/SymbolTable.cpp src/VM.cpp
OBJ := $(SRC:.cpp=.o)

.PHONY: all clean run test

all: compiler vm

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

compiler: $(OBJ) src/mainCompiler.o
	$(CXX) $(CXXFLAGS) -o $@ $^

vm: $(OBJ) src/mainVM.o
	$(CXX) $(CXXFLAGS) -o $@ $^

run: compiler
	./compiler

test: vm
	./vm

clean:
	rm -f $(OBJ) src/mainCompiler.o src/mainVM.o compiler vm
