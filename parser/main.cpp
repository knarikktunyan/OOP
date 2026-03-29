// #include "Node.hpp"
// #include <iostream>
// #include <sstream>
// #include <vector>

// std::vector<std::string> splitInput(std::stringstream& ss);
// std::vector<Token> makeTokens(const std::vector<std::string>& words);
// Node* buildTree(std::vector<Token>& tokens, std::vector<int>& values);
// int compute(const Node* n);

// void freeTree(Node* n)
// {
// 	if (!n) return;
// 	freeTree(n->left);
// 	freeTree(n->right);
// 	delete n;
// }

// int main()
// {
// 	Node* root = nullptr;

// 	try
// 	{
// 		std::string expr;
// 		std::cout << "Enter expression: ";
// 		std::getline(std::cin, expr);

// 		std::cout << "Enter variables: ";
// 		std::vector<int> values;
// 		int x;
// 		while (std::cin >> x)
// 			values.push_back(x);

// 		std::stringstream ss(expr);

// 		auto words = splitInput(ss);
// 		auto tokens = makeTokens(words);

// 		root = buildTree(tokens, values);

// 		std::cout << "Result: " << compute(root) << "\n";
// 	}
// 	catch (std::exception& e)
// 	{
// 		std::cout << e.what() << "\n";
// 	}

// 	freeTree(root);
// }

#include "Node.hpp"
#include "Token.hpp"
#include <iostream>
#include <sstream>
#include <vector>

std::vector<std::string> splitInput(std::stringstream& ss);
std::vector<Token> makeTokens(const std::vector<std::string>& words);
Node* buildTree(std::vector<Token>& tokens, std::vector<int>& values);
int compute(const Node* n);

void freeTree(Node* n)
{
    if (!n) return;
    freeTree(n->left);
    freeTree(n->right);
    delete n;
}

int main()
{
    Node* root = nullptr;

    try
    {
        std::string expr;
        std::cout << "Enter expression: ";
        std::getline(std::cin, expr);

        std::cout << "Enter variables: ";
        std::vector<int> values;
        int value;
        while (std::cin >> value)
            values.push_back(value);

        std::stringstream ss(expr);

        auto words = splitInput(ss);
        auto tokens = makeTokens(words);

        root = buildTree(tokens, values);

        std::cout << "Result: " << compute(root) << "\n";
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << "\n";
    }

    freeTree(root);
}