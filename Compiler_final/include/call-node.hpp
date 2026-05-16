#pragma once

#include "ast-node.hpp"

class CallExpressionNode : public AstNode
{
public:
	std::string name;
	std::vector<std::unique_ptr<AstNode>> args;

	CallExpressionNode() : AstNode(AstNodeType::FunctionCall) { }
	~CallExpressionNode() = default;
};