#include "../parser/Token.hpp"
#include "../parser/Node.hpp"
#include "../parser/State.hpp"
#include "Compiler.hpp"
#include <stdexcept>

int execute(const std::vector<Instruction>& prog, VM& vm)
{
    for (const auto& instr : prog)
    {
        int lhs = vm.regs[instr.left];
        int rhs = vm.regs[instr.right];

        switch (instr.op)
        {
            case Operator::Add:
                vm.regs[instr.dest] = lhs + rhs;
                break;

            case Operator::Sub:
                vm.regs[instr.dest] = lhs - rhs;
                break;

            case Operator::Mult:
                vm.regs[instr.dest] = lhs * rhs;
                break;

            case Operator::Div:
                if (rhs == 0)
                    throw std::runtime_error("Division by zero");
                vm.regs[instr.dest] = lhs / rhs;
                break;
        }
    }

    return prog.empty() ? 0 : vm.regs[prog.back().dest];
}