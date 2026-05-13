#include "SymbolTable.hpp"
#include <stdexcept>

namespace vl {

void Sym::push() {
    stk_.emplace_back();
}

void Sym::pop() {
    if (stk_.empty()) {
        throw std::runtime_error("sym pop");
    }
    stk_.pop_back();
}

void Sym::def(const std::string& n, int v) {
    if (stk_.empty()) {
        push();
    }
    auto& top = stk_.back();
    if (top.count(n)) {
        throw std::runtime_error("redef: " + n);
    }
    top[n] = v;
}

void Sym::set(const std::string& n, int v) {
    for (auto it = stk_.rbegin(); it != stk_.rend(); ++it) {
        auto f = it->find(n);
        if (f != it->end()) {
            f->second = v;
            return;
        }
    }
    throw std::runtime_error("unknown set: " + n);
}

int Sym::get(const std::string& n) const {
    for (auto it = stk_.rbegin(); it != stk_.rend(); ++it) {
        auto f = it->find(n);
        if (f != it->end()) {
            return f->second;
        }
    }
    throw std::runtime_error("unknown get: " + n);
}

}  // namespace vl
