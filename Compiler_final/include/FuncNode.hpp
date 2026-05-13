#pragma once

#include "BlockNode.hpp"
#include "Node.hpp"
#include <memory>
#include <string>
#include <vector>

namespace vl {

class FuncNode : public Stmt {
public:
    FuncNode(std::string name, std::vector<std::string> params, std::unique_ptr<Block> body);
    const std::string& name() const { return name_; }
    const std::vector<std::string>& params() const { return params_; }
    Block* body() { return body_.get(); }
    void exec(Vm& vm) override;

private:
    std::string name_;
    std::vector<std::string> params_;
    std::unique_ptr<Block> body_;
};

}  // namespace vl
