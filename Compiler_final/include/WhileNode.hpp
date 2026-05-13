#pragma once

#include "Node.hpp"
#include <memory>

namespace vl {

class WhileNode : public Stmt {
public:
    WhileNode(std::unique_ptr<Expr> c, std::unique_ptr<Stmt> b) : c_(std::move(c)), b_(std::move(b)) {}
    void exec(Vm& vm) override;

private:
    std::unique_ptr<Expr> c_;
    std::unique_ptr<Stmt> b_;
};

}  // namespace vl
