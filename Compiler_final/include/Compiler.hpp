#pragma once

#include "BlockNode.hpp"
#include <memory>
#include <string>

namespace vl {

struct ParseOut {
    std::unique_ptr<Block> ast;
    std::string err;
};

struct Compiler {
    static ParseOut compile(const std::string& src);
    static std::string slurp(const std::string& path);
};

}  // namespace vl
