#include "Compiler.hpp"
#include "VM.hpp"

#include <iostream>

int main(int argc, char** argv) {
    using namespace vl;

    try {
        const std::string src =
            argc > 1 ? Compiler::slurp(argv[1]) : Compiler::slurp("src/demo.vl");

        ParseOut r = Compiler::compile(src);
        if (!r.ast) {
            std::cerr << r.err << '\n';
            return 2;
        }

        Vm vm(std::cout);
        r.ast->exec(vm);
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << '\n';
        return 1;
    }
    return 0;
}
