#include "Token.hpp"
#include <cctype>

namespace vl {

static bool id0(char c) { return std::isalpha(static_cast<unsigned char>(c)) || c == '_'; }

static bool id1(char c) { return std::isalnum(static_cast<unsigned char>(c)) || c == '_'; }

std::vector<RawLex> scan_raw(const std::string& src) {
    std::vector<RawLex> out;
    std::size_t i = 0;
    int line = 1;
    int col = 1;

    auto bump = [&]() -> char {
        if (i >= src.size()) {
            return '\0';
        }
        char c = src[i++];
        if (c == '\n') {
            ++line;
            col = 1;
        } else {
            ++col;
        }
        return c;
    };

    auto peek = [&](std::size_t a = 0) -> char {
        std::size_t p = i + a;
        return p < src.size() ? src[p] : '\0';
    };

    while (i < src.size()) {
        char c = peek();
        if (c == '\0') {
            break;
        }
        if (std::isspace(static_cast<unsigned char>(c))) {
            bump();
            continue;
        }
        if (c == '/' && peek(1) == '/') {
            bump();
            bump();
            while (peek() && peek() != '\n') {
                bump();
            }
            continue;
        }

        const int ln = line;
        const int cl = col;

        if (std::isdigit(static_cast<unsigned char>(c))) {
            std::string s;
            while (std::isdigit(static_cast<unsigned char>(peek()))) {
                s.push_back(bump());
            }
            out.push_back({s, ln, cl});
            continue;
        }

        if (id0(c)) {
            std::string s;
            s.push_back(bump());
            while (id1(peek())) {
                s.push_back(bump());
            }
            out.push_back({s, ln, cl});
            continue;
        }

        std::string two;
        two.push_back(c);
        two.push_back(peek(1));
        if (two == "==" || two == "!=" || two == "<=" || two == ">=" || two == "&&" || two == "||") {
            bump();
            bump();
            out.push_back({two, ln, cl});
            continue;
        }

        std::string one(1, bump());
        out.push_back({one, ln, cl});
    }
    return out;
}

}  // namespace vl
