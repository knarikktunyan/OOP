#include "VM.hpp"
#include "BlockNode.hpp"
#include "CallNode.hpp"
#include "FuncNode.hpp"
#include "IfNode.hpp"
#include "WhileNode.hpp"
#include "Node.hpp"

#include <stdexcept>
#include <utility>

namespace vl {

void Block::exec(Vm& vm) {
    if (!flat_) {
        vm.enter();
    }
    for (auto& s : body_) {
        s->exec(vm);
    }
    if (!flat_) {
        vm.leave();
    }
}

void IfNode::exec(Vm& vm) {
    if (c_->eval(vm) != 0) {
        t_->exec(vm);
    } else if (a_) {
        a_->exec(vm);
    }
}

void WhileNode::exec(Vm& vm) {
    while (c_->eval(vm) != 0) {
        b_->exec(vm);
    }
}

FuncNode::FuncNode(std::string name, std::vector<std::string> params, std::unique_ptr<Block> body)
    : name_(std::move(name)), params_(std::move(params)), body_(std::move(body)) {}

void FuncNode::exec(Vm& vm) {
    vm.add_fn(this);
}

CallNode::CallNode(std::string fn, std::vector<std::unique_ptr<Expr>> args)
    : fn_(std::move(fn)), args_(std::move(args)) {}

int CallNode::eval(Vm& vm) {
    std::vector<int> v;
    v.reserve(args_.size());
    for (auto& a : args_) {
        v.push_back(a->eval(vm));
    }
    return vm.call(fn_, v);
}

int Name::eval(Vm& vm) {
    return vm.get(id_);
}

int Bin::eval(Vm& vm) {
    const int l = l_->eval(vm);
    const int r = r_->eval(vm);

    switch (op_) {
        case Bop::add:
            return l + r;
        case Bop::sub:
            return l - r;
        case Bop::mul:
            return l * r;
        case Bop::div:
            if (r == 0) {
                throw std::runtime_error("div0");
            }
            return l / r;
        case Bop::eq:
            return l == r ? 1 : 0;
        case Bop::ne:
            return l != r ? 1 : 0;
        case Bop::lt:
            return l < r ? 1 : 0;
        case Bop::le:
            return l <= r ? 1 : 0;
        case Bop::gt:
            return l > r ? 1 : 0;
        case Bop::ge:
            return l >= r ? 1 : 0;
        case Bop::band:
            return (l != 0 && r != 0) ? 1 : 0;
        case Bop::bor:
            return (l != 0 || r != 0) ? 1 : 0;
    }

    return 0;
}

int Un::eval(Vm& vm) {
    const int v = x_->eval(vm);

    switch (op_) {
        case Uop::neg:
            return -v;
        case Uop::pos:
            return v;
        case Uop::Not:
            return v == 0 ? 1 : 0;
    }

    return v;
}

int Set::eval(Vm& vm) {
    const int v = rhs_->eval(vm);
    vm.set(id_, v);
    return v;
}

void ExprStmt::exec(Vm& vm) {
    (void)x_->eval(vm);
}

void Var::exec(Vm& vm) {
    int v = 0;

    if (init_) {
        v = init_->eval(vm);
    }

    vm.def(id_, v);
}

void Ret::exec(Vm& vm) {
    int v = 0;

    if (x_) {
        v = x_->eval(vm);
    }

    throw RetEx(v);
}

void Out::exec(Vm& vm) {
    vm.out() << x_->eval(vm) << '\n';
}

Vm::Vm(std::ostream& out) : out_(out) {}

void Vm::add_fn(FuncNode* f) {
    fn_[f->name()] = f;
}

FuncNode* Vm::find_fn(const std::string& n) const {
    auto it = fn_.find(n);
    return it == fn_.end() ? nullptr : it->second;
}

void Vm::enter() {
    sym_.push();
}

void Vm::leave() {
    sym_.pop();
}

void Vm::def(const std::string& n, int v) {
    sym_.def(n, v);
}

void Vm::set(const std::string& n, int v) {
    sym_.set(n, v);
}

int Vm::get(const std::string& n) const {
    return sym_.get(n);
}

int Vm::call(const std::string& n, const std::vector<int>& args) {
    FuncNode* f = find_fn(n);

    if (!f) {
        throw std::runtime_error("no fn: " + n);
    }

    if (f->params().size() != args.size()) {
        throw std::runtime_error("arity: " + n);
    }

    enter();

    for (std::size_t i = 0; i < args.size(); ++i) {
        def(f->params()[i], args[i]);
    }

    try {
        f->body()->exec(*this);
    } catch (const RetEx& e) {
        leave();
        return e.v;
    }

    leave();
    return 0;
}

}  // namespace vl