#pragma once

#include "NodeType.hpp"
#include <string>
#include <utility>
#include <vector>

namespace vl {

struct RawLex {
    std::string t;
    int line{1};
    int col{1};
};

struct Token {
    Tok k{Tok::bad};
    std::string lex;
    int line{1};
    int col{1};
};

std::vector<RawLex> scan_raw(const std::string& src);
std::vector<Token> tokenize(const std::string& src);

}  // namespace vl
