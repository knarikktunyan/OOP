COMPILER_BINARY = Compiler
VIRTUAL_MACHINE_BINARY = VirtualMachine

CXX = c++
CXXFLAGS = -Wall -Wextra -std=c++17 -Iinclude

SRC_DIR = src

COMPILER_SRC = $(SRC_DIR)/main-compiler.cpp $(SRC_DIR)/lexer.cpp $(SRC_DIR)/tokenizer.cpp $(SRC_DIR)/parser.cpp $(SRC_DIR)/virtual-machine.cpp $(SRC_DIR)/symbol-table.cpp
COMPILER_OBJ = $(COMPILER_SRC:.cpp=.o)

VM_SRC = $(SRC_DIR)/main-virtual-machine.cpp $(SRC_DIR)/virtual-machine.cpp $(SRC_DIR)/symbol-table.cpp
VM_OBJ = $(VM_SRC:.cpp=.o)

all: $(COMPILER_BINARY) $(VIRTUAL_MACHINE_BINARY)

$(COMPILER_BINARY): $(COMPILER_OBJ)
	$(CXX) $(CXXFLAGS) $(COMPILER_OBJ) -o $(COMPILER_BINARY)

$(VIRTUAL_MACHINE_BINARY): $(VM_OBJ)
	$(CXX) $(CXXFLAGS) $(VM_OBJ) -o $(VIRTUAL_MACHINE_BINARY)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(COMPILER_OBJ) $(VM_OBJ)

fclean: clean
	rm -f $(COMPILER_BINARY) $(VIRTUAL_MACHINE_BINARY)

re: fclean all

.PHONY: all clean fclean re