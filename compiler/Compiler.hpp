#pragma once
#include "../parser/NodeType.hpp"
#include <vector>

struct Instruction
{
    Operator op;
    int dest;
    int left;
    int right;
};

struct VM
{
    int regs[100] = {0}; // KEEP SAME
    int next = 0;
};

int compile(Node* node, std::vector<Instruction>& prog, VM& vm);
int execute(const std::vector<Instruction>& prog, VM& vm);