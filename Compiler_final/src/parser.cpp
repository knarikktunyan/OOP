#include "Compiler.hpp"
#include "State.hpp"
#include "Token.hpp"
#include "Node.hpp"
#include "BlockNode.hpp"
#include "IfNode.hpp"
#include "WhileNode.hpp"
#include "FuncNode.hpp"
#include "CallNode.hpp"

#include <fstream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

namespace vl {

static Bop toBop(Tok t) {
    switch (t) {
        case Tok::plus:
            return Bop::add;
        case Tok::minus:
            return Bop::sub;
        case Tok::star:
            return Bop::mul;
        case Tok::slash:
            return Bop::div;
        case Tok::eq:
            return Bop::eq;
        case Tok::ne:
            return Bop::ne;
        case Tok::lt:
            return Bop::lt;
        case Tok::le:
            return Bop::le;
        case Tok::gt:
            return Bop::gt;
        case Tok::ge:
            return Bop::ge;
        case Tok::land:
            return Bop::band;
        case Tok::lor:
            return Bop::bor;
        default:
            throw std::runtime_error("bop");
    }
}

class Rd {
public:
    explicit Rd(Cursor c) : cur_(std::move(c)) {}

    std::unique_ptr<Block> root() {
        auto file = std::make_unique<Block>(false);
        while (!cur_.end()) {
            auto s = decl_();
            if (!s) {
                if (cur_.bad()) {
                    break;
                }
                if (!cur_.end()) {
                    cur_.fail("bad decl");
                }
                break;
            }
            file->push(std::move(s));
        }
        if (!cur_.end() && !cur_.bad()) {
            cur_.fail("trailing");
        }
        return file;
    }

    bool ok() const { return !cur_.bad(); }
    const std::string& msg() const { return cur_.err(); }

private:
    Cursor cur_;

    void err(const std::string& m) { cur_.fail(m); }

    std::unique_ptr<Stmt> decl_() {
        if (cur_.peek().k == Tok::kw_fun) {
            return fun_();
        }
        if (cur_.peek().k == Tok::kw_var) {
            return var_();
        }
        return stmt_();
    }

    std::unique_ptr<Stmt> fun_() {
        cur_.next();
        if (cur_.peek().k != Tok::id) {
            err("fn name");
            return nullptr;
        }
        std::string fname = cur_.peek().lex;
        cur_.next();
        cur_.need(Tok::lpar, "fn (");
        if (cur_.bad()) {
            return nullptr;
        }
        std::vector<std::string> params;
        if (cur_.peek().k != Tok::rpar) {
            for (;;) {
                if (cur_.peek().k != Tok::id) {
                    err("param");
                    return nullptr;
                }
                params.push_back(cur_.peek().lex);
                cur_.next();
                if (cur_.eat(Tok::comma)) {
                    continue;
                }
                break;
            }
        }
        cur_.need(Tok::rpar, "fn )");
        if (cur_.bad()) {
            return nullptr;
        }
        auto body = block_(true);
        if (!body) {
            return nullptr;
        }
        return std::unique_ptr<Stmt>(
            std::make_unique<FuncNode>(std::move(fname), std::move(params), std::move(body)).release());
    }

    std::unique_ptr<Stmt> var_() {
        cur_.next();
        if (cur_.peek().k != Tok::id) {
            err("var name");
            return nullptr;
        }
        std::string name = cur_.peek().lex;
        cur_.next();
        std::unique_ptr<Expr> init;
        if (cur_.eat(Tok::assign)) {
            init = expr_();
            if (!init || cur_.bad()) {
                return nullptr;
            }
        }
        cur_.need(Tok::semi, "var ;");
        if (cur_.bad()) {
            return nullptr;
        }
        return std::unique_ptr<Stmt>(std::make_unique<Var>(std::move(name), std::move(init)).release());
    }

    std::unique_ptr<Stmt> stmt_() {
        switch (cur_.peek().k) {
            case Tok::lbra: {
                auto b = block_(false);
                return b ? std::unique_ptr<Stmt>(b.release()) : nullptr;
            }
            case Tok::kw_if:
                return if_();
            case Tok::kw_while:
                return wh_();
            case Tok::kw_return:
                return ret_();
            case Tok::kw_print:
                return out_();
            case Tok::kw_var:
                return var_();
            default:
                return es_();
        }
    }

    std::unique_ptr<Block> block_(bool flat) {
        cur_.need(Tok::lbra, "{");
        if (cur_.bad()) {
            return nullptr;
        }
        auto b = std::make_unique<Block>(flat);
        while (cur_.peek().k != Tok::rbra && !cur_.end()) {
            auto s = decl_();
            if (!s) {
                break;
            }
            b->push(std::move(s));
        }
        cur_.need(Tok::rbra, "}");
        if (cur_.bad()) {
            return nullptr;
        }
        return b;
    }

    std::unique_ptr<Stmt> if_() {
        cur_.next();
        cur_.need(Tok::lpar, "if (");
        if (cur_.bad()) {
            return nullptr;
        }
        auto c = expr_();
        if (!c) {
            return nullptr;
        }
        cur_.need(Tok::rpar, "if )");
        if (cur_.bad()) {
            return nullptr;
        }
        auto t = stmt_();
        if (!t) {
            return nullptr;
        }
        std::unique_ptr<Stmt> a;
        if (cur_.eat(Tok::kw_else)) {
            a = stmt_();
            if (!a) {
                return nullptr;
            }
        }
        return std::unique_ptr<Stmt>(
            std::make_unique<IfNode>(std::move(c), std::move(t), std::move(a)).release());
    }

    std::unique_ptr<Stmt> wh_() {
        cur_.next();
        cur_.need(Tok::lpar, "while (");
        if (cur_.bad()) {
            return nullptr;
        }
        auto c = expr_();
        if (!c) {
            return nullptr;
        }
        cur_.need(Tok::rpar, "while )");
        if (cur_.bad()) {
            return nullptr;
        }
        auto body = stmt_();
        if (!body) {
            return nullptr;
        }
        return std::unique_ptr<Stmt>(
            std::make_unique<WhileNode>(std::move(c), std::move(body)).release());
    }

    std::unique_ptr<Stmt> ret_() {
        cur_.next();
        std::unique_ptr<Expr> v;
        if (cur_.peek().k != Tok::semi) {
            v = expr_();
            if (!v) {
                return nullptr;
            }
        }
        cur_.need(Tok::semi, "ret ;");
        if (cur_.bad()) {
            return nullptr;
        }
        return std::unique_ptr<Stmt>(std::make_unique<Ret>(std::move(v)).release());
    }

    std::unique_ptr<Stmt> out_() {
        cur_.next();
        auto x = expr_();
        if (!x) {
            return nullptr;
        }
        cur_.need(Tok::semi, "out ;");
        if (cur_.bad()) {
            return nullptr;
        }
        return std::unique_ptr<Stmt>(std::make_unique<Out>(std::move(x)).release());
    }

    std::unique_ptr<Stmt> es_() {
        auto x = expr_();
        if (!x) {
            return nullptr;
        }
        cur_.need(Tok::semi, "es ;");
        if (cur_.bad()) {
            return nullptr;
        }
        return std::unique_ptr<Stmt>(std::make_unique<ExprStmt>(std::move(x)).release());
    }

    std::unique_ptr<Expr> expr_() { return asg_(); }

    std::unique_ptr<Expr> asg_() {
        if (cur_.peek().k == Tok::id && cur_.ahead(1).k == Tok::assign) {
            std::string id = cur_.peek().lex;
            cur_.next();
            cur_.next();
            auto rhs = asg_();
            if (!rhs) {
                return nullptr;
            }
            return std::make_unique<Set>(std::move(id), std::move(rhs));
        }
        return or_();
    }

    std::unique_ptr<Expr> or_() {
        auto x = and_();
        if (!x) {
            return nullptr;
        }
        while (cur_.peek().k == Tok::lor) {
            cur_.next();
            auto r = and_();
            if (!r) {
                return nullptr;
            }
            x = std::make_unique<Bin>(std::move(x), Bop::bor, std::move(r));
        }
        return x;
    }

    std::unique_ptr<Expr> and_() {
        auto x = eq_();
        if (!x) {
            return nullptr;
        }
        while (cur_.peek().k == Tok::land) {
            cur_.next();
            auto r = eq_();
            if (!r) {
                return nullptr;
            }
            x = std::make_unique<Bin>(std::move(x), Bop::band, std::move(r));
        }
        return x;
    }

    std::unique_ptr<Expr> eq_() {
        auto x = cmp_();
        if (!x) {
            return nullptr;
        }
        while (cur_.peek().k == Tok::eq || cur_.peek().k == Tok::ne) {
            Tok op = cur_.peek().k;
            cur_.next();
            auto r = cmp_();
            if (!r) {
                return nullptr;
            }
            x = std::make_unique<Bin>(std::move(x), toBop(op), std::move(r));
        }
        return x;
    }

    std::unique_ptr<Expr> cmp_() {
        auto x = term_();
        if (!x) {
            return nullptr;
        }
        while (cur_.peek().k == Tok::lt || cur_.peek().k == Tok::le || cur_.peek().k == Tok::gt ||
               cur_.peek().k == Tok::ge) {
            Tok op = cur_.peek().k;
            cur_.next();
            auto r = term_();
            if (!r) {
                return nullptr;
            }
            x = std::make_unique<Bin>(std::move(x), toBop(op), std::move(r));
        }
        return x;
    }

    std::unique_ptr<Expr> term_() {
        auto x = fac_();
        if (!x) {
            return nullptr;
        }
        while (cur_.peek().k == Tok::plus || cur_.peek().k == Tok::minus) {
            Tok op = cur_.peek().k;
            cur_.next();
            auto r = fac_();
            if (!r) {
                return nullptr;
            }
            x = std::make_unique<Bin>(std::move(x), toBop(op), std::move(r));
        }
        return x;
    }

    std::unique_ptr<Expr> fac_() {
        auto x = un_();
        if (!x) {
            return nullptr;
        }
        while (cur_.peek().k == Tok::star || cur_.peek().k == Tok::slash) {
            Tok op = cur_.peek().k;
            cur_.next();
            auto r = un_();
            if (!r) {
                return nullptr;
            }
            x = std::make_unique<Bin>(std::move(x), toBop(op), std::move(r));
        }
        return x;
    }

    std::unique_ptr<Expr> un_() {
        if (cur_.eat(Tok::minus)) {
            auto i = un_();
            if (!i) {
                return nullptr;
            }
            return std::make_unique<Un>(Uop::neg, std::move(i));
        }
        if (cur_.eat(Tok::plus)) {
            auto i = un_();
            if (!i) {
                return nullptr;
            }
            return std::make_unique<Un>(Uop::pos, std::move(i));
        }
        if (cur_.eat(Tok::bang)) {
            auto i = un_();
            if (!i) {
                return nullptr;
            }
            return std::make_unique<Un>(Uop::Not, std::move(i));
        }
        return post_();
    }

    std::unique_ptr<Expr> post_() {
        auto x = prim_();
        if (!x) {
            return nullptr;
        }
        while (cur_.peek().k == Tok::lpar) {
            auto* nm = dynamic_cast<Name*>(x.get());
            if (!nm) {
                err("call");
                return nullptr;
            }
            std::string fn = nm->id();
            cur_.next();
            std::vector<std::unique_ptr<Expr>> args;
            if (cur_.peek().k != Tok::rpar) {
                for (;;) {
                    auto a = expr_();
                    if (!a) {
                        return nullptr;
                    }
                    args.push_back(std::move(a));
                    if (cur_.eat(Tok::comma)) {
                        continue;
                    }
                    break;
                }
            }
            cur_.need(Tok::rpar, "call )");
            if (cur_.bad()) {
                return nullptr;
            }
            x = std::make_unique<CallNode>(std::move(fn), std::move(args));
        }
        return x;
    }

    std::unique_ptr<Expr> prim_() {
        if (cur_.peek().k == Tok::num) {
            int v = std::stoi(cur_.peek().lex);
            cur_.next();
            return std::make_unique<Lit>(v);
        }
        if (cur_.peek().k == Tok::id) {
            std::string id = cur_.peek().lex;
            cur_.next();
            return std::make_unique<Name>(std::move(id));
        }
        if (cur_.eat(Tok::lpar)) {
            auto x = expr_();
            if (!x) {
                return nullptr;
            }
            cur_.need(Tok::rpar, "grp");
            if (cur_.bad()) {
                return nullptr;
            }
            return x;
        }
        err("prim");
        return nullptr;
    }
};

static ParseOut parse_go(Cursor cur) {
    Rd p(std::move(cur));
    ParseOut o;
    o.ast = p.root();
    o.err = p.msg();
    if (!p.ok()) {
        o.ast.reset();
        if (o.err.empty()) {
            o.err = "parse";
        }
    }
    return o;
}

ParseOut Compiler::compile(const std::string& src) {
    return parse_go(Cursor(tokenize(src)));
}

std::string Compiler::slurp(const std::string& path) {
    std::ifstream in(path);
    if (!in) {
        throw std::runtime_error("open: " + path);
    }
    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

}  // namespace vl
