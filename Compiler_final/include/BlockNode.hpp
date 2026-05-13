#pragma once

#include "Node.hpp"
#include <memory>
#include <vector>

namespace vl {

class Block : public Stmt {
public:
    explicit Block(bool flat = false) : flat_(flat) {}
    void push(std::unique_ptr<Stmt> s) { body_.push_back(std::move(s)); }
    void exec(Vm& vm) override;
    const std::vector<std::unique_ptr<Stmt>>& body() const { return body_; }

private:
    std::vector<std::unique_ptr<Stmt>> body_;
    bool flat_{false};
};

}  // namespace vl
