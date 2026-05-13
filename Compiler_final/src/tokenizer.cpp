#include "Token.hpp"
#include <cctype>
#include <unordered_map>

namespace vl {

static bool id0(char c) { return std::isalpha(static_cast<unsigned char>(c)) || c == '_'; }

static Tok word(const std::string& w) {
    static const std::unordered_map<std::string, Tok> kw = {
        {"var", Tok::kw_var},     {"fun", Tok::kw_fun},         {"if", Tok::kw_if},
        {"else", Tok::kw_else},   {"while", Tok::kw_while},     {"return", Tok::kw_return},
        {"print", Tok::kw_print},
    };
    auto it = kw.find(w);
    return it == kw.end() ? Tok::id : it->second;
}

static Tok punct(const std::string& s) {
    if (s == "(") {
        return Tok::lpar;
    }
    if (s == ")") {
        return Tok::rpar;
    }
    if (s == "{") {
        return Tok::lbra;
    }
    if (s == "}") {
        return Tok::rbra;
    }
    if (s == ";") {
        return Tok::semi;
    }
    if (s == ",") {
        return Tok::comma;
    }
    if (s == "+") {
        return Tok::plus;
    }
    if (s == "-") {
        return Tok::minus;
    }
    if (s == "*") {
        return Tok::star;
    }
    if (s == "/") {
        return Tok::slash;
    }
    if (s == "=") {
        return Tok::assign;
    }
    if (s == "==") {
        return Tok::eq;
    }
    if (s == "!=") {
        return Tok::ne;
    }
    if (s == "<") {
        return Tok::lt;
    }
    if (s == "<=") {
        return Tok::le;
    }
    if (s == ">") {
        return Tok::gt;
    }
    if (s == ">=") {
        return Tok::ge;
    }
    if (s == "&&") {
        return Tok::land;
    }
    if (s == "||") {
        return Tok::lor;
    }
    if (s == "!") {
        return Tok::bang;
    }
    return Tok::bad;
}

std::vector<Token> tokenize(const std::string& src) {
    std::vector<Token> out;
    int el = 1;
    int ec = 1;

    for (const auto& r : scan_raw(src)) {
        el = r.line;
        ec = r.col;
        Tok k = Tok::bad;
        if (!r.t.empty() && std::isdigit(static_cast<unsigned char>(r.t[0]))) {
            k = Tok::num;
        } else if (!r.t.empty() && (id0(r.t[0]))) {
            k = word(r.t);
        } else {
            k = punct(r.t);
        }
        out.push_back(Token{k, r.t, r.line, r.col});
    }
    out.push_back(Token{Tok::eof, "", el, ec});
    return out;
}

}  // namespace vl
