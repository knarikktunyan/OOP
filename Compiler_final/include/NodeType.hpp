#pragma once

namespace vl {

/// Token kinds after scanning + keyword resolution.
enum class Tok {
    eof,
    id,
    num,
    kw_var,
    kw_fun,
    kw_if,
    kw_else,
    kw_while,
    kw_return,
    kw_print,
    lpar,
    rpar,
    lbra,
    rbra,
    semi,
    comma,
    plus,
    minus,
    star,
    slash,
    assign,
    eq,
    ne,
    lt,
    le,
    gt,
    ge,
    land,
    lor,
    bang,
    bad
};

enum class Bop { add, sub, mul, div, eq, ne, lt, le, gt, ge, band, bor };

enum class Uop { neg, pos, Not };

}  // namespace vl
