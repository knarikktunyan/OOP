#pragma once
#include "../parser/NodeType.hpp"
#include "../parser/Node.hpp"
#include <vector>
#include <cstdint>


// bytecode operations
enum OpCode
{
    LOAD_NUM,
    LOAD_VAR,
    ADD,
    SUB,
    MUL,
    DIV
};


struct Instruction
{
    OpCode op;
    int dest;
    intptr_t left;
    intptr_t right;
};

struct VM
{
    int regs[100] = {0};
    int next = 0;
};

int compile(Node* node, std::vector<Instruction>& prog, VM& vm);
int execute(const std::vector<Instruction>& prog, VM& vm);