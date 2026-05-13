// #pragma once

// #include "SymbolTable.hpp"
// #include <ostream>
// #include <stdexcept>
// #include <string>
// #include <unordered_map>
// #include <vector>

// namespace vl {

// class FuncNode;

// struct RetEx : std::runtime_error {
//     int v;
//     explicit RetEx(int x) : std::runtime_error("ret"), v(x) {}
// };

// /// Tree-walk interpreter (the "VM" for this project).
// class Vm {
// public:
//     explicit Vm(std::ostream& out) : out_(out) {}

//     void add_fn(FuncNode* f);
//     FuncNode* find_fn(const std::string& n) const;

//     void enter();
//     void leave();
//     void def(const std::string& n, int v);
//     void set(const std::string& n, int v);
//     int get(const std::string& n) const;

//     int call(const std::string& n, const std::vector<int>& args);

//     std::ostream& out() { return out_; }

// private:
//     Sym sym_;
//     std::unordered_map<std::string, FuncNode*> fn_;
//     std::ostream& out_;
// };

// }  // namespace vl
// include/VM.hpp
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