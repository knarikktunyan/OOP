#include "../parser/Token.hpp"
#include "../parser/Node.hpp"
#include "../parser/State.hpp"
#include "Compiler.hpp"
#include <stdexcept>

int execute(const std::vector<Instruction>& prog, VM& vm)
{
    for (const auto& instr : prog)
    {
        switch (instr.op)
        {
            case LOAD_NUM:
                vm.regs[instr.dest] = (int)instr.left;
                break;

            case LOAD_VAR:
                vm.regs[instr.dest] = *(int*)instr.left;
                break;

            case ADD:
                vm.regs[instr.dest] =
                    vm.regs[instr.left] + vm.regs[instr.right];
                break;

            case SUB:
                vm.regs[instr.dest] =
                    vm.regs[instr.left] - vm.regs[instr.right];
                break;

            case MUL:
                vm.regs[instr.dest] =
                    vm.regs[instr.left] * vm.regs[instr.right];
                break;

            case DIV:
                if (vm.regs[instr.right] == 0)
                    throw std::runtime_error("Division by zero");

                vm.regs[instr.dest] =
                    vm.regs[instr.left] / vm.regs[instr.right];
                break;
        }
    }

    return prog.empty() ? 0 : vm.regs[prog.back().dest];
}