#pragma once

#include "Node.hpp"
#include <memory>

namespace vl {

class IfNode : public Stmt {
public:
    IfNode(std::unique_ptr<Expr> c, std::unique_ptr<Stmt> t, std::unique_ptr<Stmt> a = nullptr)
        : c_(std::move(c)), t_(std::move(t)), a_(std::move(a)) {}
    void exec(Vm& vm) override;

private:
    std::unique_ptr<Expr> c_;
    std::unique_ptr<Stmt> t_;
    std::unique_ptr<Stmt> a_;
};

}  // namespace vl
