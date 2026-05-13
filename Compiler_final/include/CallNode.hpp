#pragma once

#include "Node.hpp"
#include <memory>
#include <string>
#include <vector>

namespace vl {

class CallNode : public Expr {
public:
    CallNode(std::string fn, std::vector<std::unique_ptr<Expr>> args);
    int eval(Vm& vm) override;

private:
    std::string fn_;
    std::vector<std::unique_ptr<Expr>> args_;
};

}  // namespace vl
