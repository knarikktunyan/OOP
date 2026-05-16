#pragma once

#include "ast-node-type.hpp"
#include <stack>
#include "token.hpp"
#include <sstream>
#include <memory>

class AstNode
{
public:
	AstNodeType type;
	std::string name;

	std::unique_ptr<AstNode> left;
	std::unique_ptr<AstNode> right;

	int value = 0;
	size_t symbolAddress = 0;
	ArithmeticOperator op;

public:
	AstNode(AstNodeType t) : type(t) {}

	AstNode(const Token& token)
	{
		type = token.type;
		name = token.value;

		switch (token.type)
		{
			case AstNodeType::Number:
				value = std::atoi(token.value.c_str());
				break;
			case AstNodeType::Variable:
				name = token.value;
				break;
			case AstNodeType::BinaryOperator:
				if (token.value == "+") op = ArithmeticOperator::Addition;
				else if (token.value == "-") op = ArithmeticOperator::Subtraction;
				else if (token.value == "*") op = ArithmeticOperator::Multiplication;
				else if (token.value == "/") op = ArithmeticOperator::Division;
				break;
			default:
				break;
		}
	}
	virtual ~AstNode() = default;
};
