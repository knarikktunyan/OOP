#pragma once

#include "ast-node.hpp"

class BlockStatementNode : public AstNode
{
public:
	std::vector<std::unique_ptr<AstNode>> statements;

	BlockStatementNode() : AstNode(AstNodeType::BlockStatement) { }
	~BlockStatementNode() = default;
};