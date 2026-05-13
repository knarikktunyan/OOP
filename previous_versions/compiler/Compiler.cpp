#include "../parser/Token.hpp"
#include "../parser/Node.hpp"
#include "../parser/State.hpp"
#include "Compiler.hpp"

int compile(Node* node, std::vector<Instruction>& prog, VM& vm)
{
    if (node->type == Num)
    {
        int r = vm.next++;
        prog.push_back({LOAD_NUM, r, node->value, 0});
        return r;
    }

    if (node->type == Var)
    {
        int r = vm.next++;
        prog.push_back({LOAD_VAR, r, (intptr_t)(node->ptr), 0});
        return r;
    }

    int leftReg  = compile(node->left, prog, vm);
    int rightReg = compile(node->right, prog, vm);

    int destReg = vm.next++;

    OpCode op;
    switch (node->op)
    {
        case Add:  op = ADD; break;
        case Sub:  op = SUB; break;
        case Mult: op = MUL; break;
        case Div:  op = DIV; break;
    }

    prog.push_back({op, destReg, leftReg, rightReg});
    return destReg;
}