#pragma once

#include "Token.hpp"
#include <cstddef>
#include <string>
#include <utility>
#include <vector>

namespace vl {

/// Current token position while parsing.
class Cursor {
public:
    explicit Cursor(std::vector<Token> toks) : t_(std::move(toks)) {}

    const Token& peek() const {
        if (i_ >= t_.size()) {
            static const Token eof{Tok::eof, "", 1, 1};
            return eof;
        }
        return t_[i_];
    }

    const Token& ahead(std::size_t n) const {
        std::size_t j = i_ + n;
        if (j >= t_.size()) {
            static const Token eof{Tok::eof, "", 1, 1};
            return eof;
        }
        return t_[j];
    }

    bool end() const { return peek().k == Tok::eof; }

    void next() {
        if (!end()) {
            ++i_;
        }
    }

    bool eat(Tok k) {
        if (peek().k == k) {
            next();
            return true;
        }
        return false;
    }

    void need(Tok k, const char* ctx) {
        if (peek().k != k) {
            fail(std::string("expected token: ") + ctx);
            return;
        }
        next();
    }

    bool bad() const { return !err_.empty(); }
    const std::string& err() const { return err_; }

    void fail(std::string m) {
        if (!err_.empty()) {
            return;
        }
        err_ = std::move(m);
    }

private:
    std::vector<Token> t_;
    std::size_t i_{0};
    std::string err_;
};

}  // namespace vl
