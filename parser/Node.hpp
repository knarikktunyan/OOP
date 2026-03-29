// #pragma once

// #include <string>
// #include <cstdlib>
// #include "NodeType.hpp"
// #include "Token.hpp"

// struct Node
// {
// 	int value;
// 	int* ptr;
// 	Operator op;
// 	NodeType type;

// 	Node* left;
// 	Node* right;

// 	std::string name;

// 	Node(const Token& tok)
// 	{
// 		left = nullptr;
// 		right = nullptr;
// 		ptr = nullptr;
// 		value = 0;
// 		name = tok.value;
// 		type = tok.type;

// 		if (type == Num)
// 			value = std::atoi(tok.value.c_str());

// 		else if (type == Op)
// 		{
// 			if (tok.value == "+") op = Add;
// 			else if (tok.value == "-") op = Sub;
// 			else if (tok.value == "*") op = Mult;
// 			else if (tok.value == "/") op = Div;
// 		}
// 	}
// };

#pragma once
#include <string>
#include <cstdlib>
#include "NodeType.hpp"
#include "Token.hpp"

struct Node
{
    int value;
    int* ptr;
    Operator op;
    NodeType type;

    Node* left;
    Node* right;

    std::string name;

    Node(const Token& tok)
        : value(0), ptr(nullptr), left(nullptr), right(nullptr),
          name(tok.value), type(tok.type)
    {
        if (type == Num)
        {
            value = std::atoi(tok.value.c_str());
        }
        else if (type == Op)
        {
            if (tok.value == "+") op = Add;
            else if (tok.value == "-") op = Sub;
            else if (tok.value == "*") op = Mult;
            else if (tok.value == "/") op = Div;
        }
    }
};