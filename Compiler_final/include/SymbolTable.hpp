#pragma once

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace vl {

/// Scoped locals: stack of frames, each a flat map.
class Sym {
public:
    void push();
    void pop();

    void def(const std::string& n, int v);
    void set(const std::string& n, int v);
    int get(const std::string& n) const;

private:
    std::vector<std::unordered_map<std::string, int>> stk_;
};

}  // namespace vl
