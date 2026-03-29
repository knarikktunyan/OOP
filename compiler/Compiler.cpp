#include "../parser/Token.hpp"
#include "../parser/Node.hpp"
#include "../parser/State.hpp"
#include "Compiler.hpp"

int compile(Node* node, std::vector<Instruction>& prog, VM& vm)
{
    if (node->type == NodeType::Num)
    {
        int regIndex = vm.next++;
        vm.regs[regIndex] = node->value;
        return regIndex;
    }

    if (node->type == NodeType::Var)
    {
        int regIndex = vm.next++;
        vm.regs[regIndex] = *(node->ptr);
        return regIndex;
    }

    int leftReg  = compile(node->left, prog, vm);
    int rightReg = compile(node->right, prog, vm);

    int destReg = vm.next++;

    Operator op = node->op;

    prog.push_back({op, destReg, leftReg, rightReg});

    return destReg;
}