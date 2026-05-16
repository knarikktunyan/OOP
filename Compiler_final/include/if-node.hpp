#pragma once

#include "ast-node.hpp"

class IfStatementNode : public AstNode
{
public:
	std::unique_ptr<AstNode> condition;
	std::unique_ptr<AstNode> trueBranch;
	std::unique_ptr<AstNode> falseBranch;

	IfStatementNode() : AstNode(AstNodeType::If) { }
	~IfStatementNode() = default;
};