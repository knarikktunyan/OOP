#include "Compiler.hpp"
#include "VM.hpp"

#include <cassert>
#include <sstream>
#include <stdexcept>
#include <string>

namespace {

std::string run(const std::string& code) {
    vl::ParseOut r = vl::Compiler::compile(code);
    if (!r.ast) {
        throw std::runtime_error(r.err);
    }
    std::ostringstream oss;
    vl::Vm vm(oss);
    r.ast->exec(vm);
    return oss.str();
}

}  // namespace

int main() {
    using namespace std::string_literals;

    assert(run("var x = 2; print x * 3 + 4;") == "10\n"s);
    assert(run("fun inc(n) { return n + 1; } var y = inc(5); print y;") == "6\n"s);
    assert(run("var t = 1; if (t) { print 42; } else { print 0; }") == "42\n"s);
    assert(run("var n = 3; while (n) { print n; n = n - 1; }") == "3\n2\n1\n"s);

    return 0;
}
