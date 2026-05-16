#pragma once

#include "ast-node.hpp"

class WhileStatementNode : public AstNode
{
public:
	std::unique_ptr<AstNode> condition;
	std::unique_ptr<AstNode> body;

	WhileStatementNode() : AstNode(AstNodeType::While) { }
	~WhileStatementNode() = default;
};