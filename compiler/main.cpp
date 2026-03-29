#include "../parser/Token.hpp"
#include "../parser/Node.hpp"
#include "../parser/State.hpp"
#include "Compiler.hpp"
#include <iostream>
#include <sstream>
#include <vector>

// parser functions
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

        // read expression
        std::cout << "Enter expression: ";
        std::getline(std::cin, expr);

        // read variables
        std::cout << "Enter variables: ";
        std::string varLine;
        std::getline(std::cin >> std::ws, varLine);

        std::stringstream vs(varLine);
        int val;
        while (vs >> val)
            vars.push_back(val);

        // process expression
        std::stringstream ss(expr);

        auto raw = splitInput(ss);
        auto tokens = makeTokens(raw);

        root = buildTree(tokens, vars);

        // run VM
        VM vm;
        std::vector<Instruction> program;

        compile(root, program, vm);

        std::cout << "\nValue: " << execute(program, vm) << "\n";
        std::cout << "Done!\n";

        freeTree(root);
    }
    catch (std::exception& e)
    {
        freeTree(root);
        std::cout << e.what() << '\n';
    }
}