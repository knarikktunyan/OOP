#pragma once

#include "NodeType.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace vl {

class Vm;

class Node {
public:
    virtual ~Node() = default;
};

class Expr : public Node {
public:
    ~Expr() override = default;
    virtual int eval(Vm& vm) = 0;
};

class Stmt : public Node {
public:
    ~Stmt() override = default;
    virtual void exec(Vm& vm) = 0;
};

class Lit : public Expr {
public:
    explicit Lit(int v) : v_(v) {}
    int eval(Vm&) override { return v_; }

private:
    int v_;
};

class Name : public Expr {
public:
    explicit Name(std::string id) : id_(std::move(id)) {}
    int eval(Vm& vm) override;
    const std::string& id() const { return id_; }

private:
    std::string id_;
};

class Bin : public Expr {
public:
    Bin(std::unique_ptr<Expr> l, Bop op, std::unique_ptr<Expr> r)
        : l_(std::move(l)), op_(op), r_(std::move(r)) {}
    int eval(Vm& vm) override;

private:
    std::unique_ptr<Expr> l_;
    Bop op_;
    std::unique_ptr<Expr> r_;
};

class Un : public Expr {
public:
    Un(Uop op, std::unique_ptr<Expr> x) : op_(op), x_(std::move(x)) {}
    int eval(Vm& vm) override;

private:
    Uop op_;
    std::unique_ptr<Expr> x_;
};

class Set : public Expr {
public:
    Set(std::string id, std::unique_ptr<Expr> rhs) : id_(std::move(id)), rhs_(std::move(rhs)) {}
    int eval(Vm& vm) override;

private:
    std::string id_;
    std::unique_ptr<Expr> rhs_;
};

class ExprStmt : public Stmt {
public:
    explicit ExprStmt(std::unique_ptr<Expr> x) : x_(std::move(x)) {}
    void exec(Vm& vm) override;

private:
    std::unique_ptr<Expr> x_;
};

class Var : public Stmt {
public:
    Var(std::string id, std::unique_ptr<Expr> init) : id_(std::move(id)), init_(std::move(init)) {}
    void exec(Vm& vm) override;

private:
    std::string id_;
    std::unique_ptr<Expr> init_;
};

class Ret : public Stmt {
public:
    explicit Ret(std::unique_ptr<Expr> x) : x_(std::move(x)) {}
    void exec(Vm& vm) override;

private:
    std::unique_ptr<Expr> x_;
};

class Out : public Stmt {
public:
    explicit Out(std::unique_ptr<Expr> x) : x_(std::move(x)) {}
    void exec(Vm& vm) override;

private:
    std::unique_ptr<Expr> x_;
};

}  // namespace vl
