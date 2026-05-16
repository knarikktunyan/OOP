#pragma once

#include "ast-node.hpp"

struct FunctionParameter
{
	std::string type;
	std::string name;
};

enum class ReturnType
{
	Int,
	Void
};

class ReturnStatementNode : public AstNode
{
public:
	std::unique_ptr<AstNode> expr;

	ReturnStatementNode() : AstNode(AstNodeType::ReturnStatement) {}
};

class FunctionNode : public AstNode
{
public:
	ReturnType returnType;
	std::vector<FunctionParameter> params;
	std::unique_ptr<BlockStatementNode> body;
	ReturnStatementNode* returnStatementNode = nullptr;

	FunctionNode() : AstNode(AstNodeType::FunctionDecl) { }
	~FunctionNode() = default;
};
