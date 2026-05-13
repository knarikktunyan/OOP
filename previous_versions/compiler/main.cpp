#include "../parser/Token.hpp"
#include "../parser/Node.hpp"
#include "../parser/State.hpp"
#include "Compiler.hpp"

#include <iostream>
#include <sstream>
#include <vector>

std::vector<std::string> splitInput(std::stringstream& ss);
std::vector<Token> makeTokens(const std::vector<std::string>& words);
Node* buildTree(std::vector<Token>& tokens, std::vector<int>& vars);

void freeTree(Node* node)
{
    if (!node) return;
    freeTree(node->left);
    freeTree(node->right);
    delete node;
}

int main()
{
    Node* root = nullptr;

    try
    {
        std::vector<int> vars;
        std::string expr;

        std::cout << "Enter expression: ";
        std::getline(std::cin, expr);

        std::cout << "Enter variables (space-separated): ";
        std::string varLine;
        std::getline(std::cin >> std::ws, varLine);

        std::stringstream vs(varLine);
        int val;
        while (vs >> val)
            vars.push_back(val);

        std::stringstream ss(expr);
        auto raw = splitInput(ss);
        auto tokens = makeTokens(raw);

        root = buildTree(tokens, vars);

        VM vm;
        std::vector<Instruction> program;

        compile(root, program, vm);

        std::cout << "\n[Instructions]\n";
        for (size_t i = 0; i < program.size(); ++i)
        {
            const auto& inst = program[i];
            std::cout << i << ": "
                      << inst.op << " "
                      << inst.dest << " "
                      << inst.left << " "
                      << inst.right << "\n";
        }

        int result = execute(program, vm);

        std::cout << "\nValue: " << result << "\n";
        std::cout << "Done!\n";

        freeTree(root);
    }
    catch (std::exception& e)
    {
        freeTree(root);
        std::cout << "Error: " << e.what() << '\n';
    }

    return 0;
}