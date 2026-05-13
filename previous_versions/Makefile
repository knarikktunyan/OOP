CXX = g++
#CXXFLAGS = -std=c++17 -Wall

COMPILER_DIR = compiler
PARSER_DIR = parser

COMPILER_SRC = $(wildcard $(COMPILER_DIR)/*.cpp)
PARSER_SRC = $(wildcard $(PARSER_DIR)/*.cpp)

PARSER_SRC := $(filter-out $(PARSER_DIR)/main.cpp, $(PARSER_SRC))

TARGET = program

all:
	$(CXX) $(CXXFLAGS) $(COMPILER_SRC) $(PARSER_SRC) -o $(TARGET)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)
