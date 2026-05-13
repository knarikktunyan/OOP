#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "SymbolTable.hpp"

namespace vl {

class FuncNode;

struct RetEx {
    int v;
    explicit RetEx(int x) : v(x) {}
};

class Vm {
    std::ostream& out_;
    Sym sym_;
    std::map<std::string, FuncNode*> fn_;

public:
    explicit Vm(std::ostream& out);

    std::ostream& out() { return out_; }

    void add_fn(FuncNode* f);
    FuncNode* find_fn(const std::string& n) const;

    void enter();
    void leave();

    void def(const std::string& n, int v);
    void set(const std::string& n, int v);
    int get(const std::string& n) const;

    int call(const std::string& n, const std::vector<int>& args);
};

}  // namespace vl