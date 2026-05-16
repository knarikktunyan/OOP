#include "../include/compiler-pipeline.hpp"

int isInsideFunction = 0;

int getOperatorPrecedence(AstNode* node)
{
	if (node->name == "*" || node->name == "/")
		return 2;
	if (node->name == "+" || node->name == "-")
		return 1;
	return 0;
}

std::unique_ptr<AstNode> parseExpression(std::vector<Token>& tokens, SymbolTable& symbolTable)
{
	ParserState state = ParserState::Start;

	std::stack<std::unique_ptr<AstNode>> operators;
	std::stack<std::unique_ptr<AstNode>> operands;

	for (const Token& t : tokens)
	{
		state = EXPRESSION_PARSER_FSM[static_cast<int>(state)][static_cast<int>(t.type)];

		if (state == ParserState::Error)
			throw std::runtime_error("unexpected token " + t.value);
		if (state == ParserState::End)
			break;

		std::unique_ptr<AstNode> n = std::make_unique<AstNode>(t);

		if (t.type == AstNodeType::Number)
			operands.push(std::move(n));
		else if (t.type == AstNodeType::Variable)
		{
			n->symbolAddress = symbolTable.getAddress(n->name);
			operands.push(std::move(n));
		}
		else if (t.type == AstNodeType::BinaryOperator || t.type == AstNodeType::Comparison)
		{
			while (!operators.empty() &&
				operators.top()->type != AstNodeType::OpenParen &&
				getOperatorPrecedence(operators.top().get()) >= getOperatorPrecedence(n.get()))
			{
				std::unique_ptr<AstNode> op = std::move(operators.top());
				operators.pop();

				if (operands.size() < 2)
					throw std::runtime_error("invalid expression");

				std::unique_ptr<AstNode> right = std::move(operands.top()); operands.pop();
				std::unique_ptr<AstNode> left  = std::move(operands.top()); operands.pop();

				op->left  = std::move(left);
				op->right = std::move(right);

				operands.push(std::move(op));
			}
			operators.push(std::move(n));
		}
		else if (t.type == AstNodeType::OpenParen)
			operators.push(std::move(n));
		else if (t.type == AstNodeType::CloseParen)
		{
			while (!operators.empty() && operators.top()->type != AstNodeType::OpenParen)
			{
				std::unique_ptr<AstNode> op = std::move(operators.top());
				operators.pop();

				if (operands.size() < 2)
					throw std::runtime_error("invalid expression");

				std::unique_ptr<AstNode> right = std::move(operands.top()); operands.pop();
				std::unique_ptr<AstNode> left  = std::move(operands.top()); operands.pop();

				op->left  = std::move(left);
				op->right = std::move(right);

				operands.push(std::move(op));
			}

			if (operators.empty())
				throw std::runtime_error("Mismatched parentheses");

			operators.pop();
		}
	}

	if (state != ParserState::End)
		throw std::runtime_error("unexpected end");

	while (!operators.empty())
	{
		std::unique_ptr<AstNode> op = std::move(operators.top());
		operators.pop();

		if (operands.size() < 2)
			throw std::runtime_error("invalid expression");

		std::unique_ptr<AstNode> right = std::move(operands.top()); operands.pop();
		std::unique_ptr<AstNode> left  = std::move(operands.top()); operands.pop();

		op->left  = std::move(left);
		op->right = std::move(right);

		operands.push(std::move(op));
	}

	if (operands.empty())
		throw std::runtime_error("Empty expression");
	return std::move(operands.top());
}

std::unique_ptr<AstNode> parseAssignment(std::vector<Token>& tokens, SymbolTable& symbolTable, int& pos)
{
	if (tokens[pos].type != AstNodeType::Variable)
		throw std::runtime_error("Expected variable name " + tokens[pos].value);
	
	if (!symbolTable.isDeclared(tokens[pos].value))
		throw std::runtime_error("Variable " + tokens[pos].value + " is not declared");
		
	auto node = std::make_unique<AstNode>(AstNodeType::Assign);
	node->left = std::make_unique<AstNode>(tokens[pos]);
	node->left->symbolAddress = symbolTable.getAddress(tokens[pos].value);

	pos++;

	if (tokens[pos].type != AstNodeType::Assign)
		throw std::runtime_error("Expected = " + tokens[pos].value);

	pos++;

	std::vector<Token> expr;
	while (tokens[pos].type != AstNodeType::Semicolon)
	{
		if (tokens[pos].type == AstNodeType::EndOfExpression)
			throw std::runtime_error("Expected ; " + tokens[pos].value);
		expr.push_back(tokens[pos++]);
	}
	expr.push_back(Token("", AstNodeType::EndOfExpression));
	node->right = parseExpression(expr, symbolTable);

	pos++;
	return node;
}

std::unique_ptr<AstNode> parseIfStatement(std::vector<Token>& tokens, SymbolTable& symbolTable, int& pos)
{
	if (tokens[pos].type != AstNodeType::If)
		throw std::runtime_error("Expected if " + tokens[pos].value);
	
	pos++;
	std::vector<Token> cond;
	
	if (tokens[pos].type != AstNodeType::OpenParen)
		throw std::runtime_error("Expected ( " + tokens[pos].value);
	
	pos++;
	
	while (tokens[pos].type != AstNodeType::CloseParen)
	{
		if (tokens[pos].type == AstNodeType::EndOfExpression)
			throw std::runtime_error("Missing )");
		cond.push_back(tokens[pos++]);
	}
	cond.push_back(Token("", AstNodeType::EndOfExpression));
	
	pos++;
	
	auto node = std::make_unique<IfStatementNode>();
	node->condition = parseExpression(cond, symbolTable);
	
	if (tokens[pos].type == AstNodeType::OpenBrace)
		node->trueBranch = parseBlock(tokens, symbolTable, pos);
	else
		node->trueBranch = parseStatement(tokens, symbolTable, pos);
	// if (tokens[pos].type != AstNodeType::OpenBrace)
	// 	throw std::runtime_error("Expected {");
	
	// node->trueBranch = parseBlock(tokens, symbolTable, pos);
	
	if (tokens[pos].type == AstNodeType::Else)
	{
		pos++;
		if (tokens[pos].type == AstNodeType::If)
			node->falseBranch = parseIfStatement(tokens, symbolTable, pos);
		else if (tokens[pos].type == AstNodeType::OpenBrace)
			node->falseBranch = parseBlock(tokens, symbolTable, pos);
		else
			node->falseBranch = parseStatement(tokens, symbolTable, pos);
	}
	return node;
}

std::unique_ptr<AstNode> parseWhileLoop(std::vector<Token>& tokens, SymbolTable& symbolTable, int& pos)
{
	if (tokens[pos].type != AstNodeType::While)
		throw std::runtime_error("Expected while " + tokens[pos].value);
	
	pos++;
	std::vector<Token> cond;
	
	if (tokens[pos].type != AstNodeType::OpenParen)
		throw std::runtime_error("Expected ( " + tokens[pos].value);
	
	pos++;
	
	while (tokens[pos].type != AstNodeType::CloseParen)
	{
		if (tokens[pos].type == AstNodeType::EndOfExpression)
			throw std::runtime_error("Expected ) " + tokens[pos].value);
		cond.push_back(tokens[pos++]);
	}
	cond.push_back(Token("", AstNodeType::EndOfExpression));
	
	pos++;
	
	auto node = std::make_unique<WhileStatementNode>();
	node->condition = parseExpression(cond, symbolTable);
	
	if (tokens[pos].type != AstNodeType::OpenBrace)
		throw std::runtime_error("Expected { " + tokens[pos].value);
	
	node->body = parseBlock(tokens, symbolTable, pos);
	
	return node;
}


std::unique_ptr<AstNode> parseVariableDeclaration(std::vector<Token>& tokens, SymbolTable& symbolTable, int& pos)
{
	if (tokens[pos].type != AstNodeType::TypeKeyword)
		throw std::runtime_error("Expected type" + tokens[pos].value);

	pos++;
	if (tokens[pos].type != AstNodeType::Variable)
		throw std::runtime_error("Expected variable name" + tokens[pos].value);
	
	symbolTable.declareVariable(tokens[pos].value);

	std::string varName = tokens[pos].value;
	
	if (tokens[pos + 1].type == AstNodeType::Assign)
		return parseAssignment(tokens, symbolTable, pos);
	
	pos++;

	if (tokens[pos].type != AstNodeType::Semicolon)
		throw std::runtime_error("Expected ';' after declaration" + tokens[pos].value);

	pos++;

	return nullptr;
}

std::unique_ptr<AstNode> parseReturnStatement(std::vector<Token>& tokens, SymbolTable& symbolTable, int& pos)
{
	if (tokens[pos].type != AstNodeType::ReturnStatement)
		throw std::runtime_error("Expected return");

	pos++;

	auto node = std::make_unique<ReturnStatementNode>();

	// return;
	if (tokens[pos].type == AstNodeType::Semicolon)
	{
		pos++;
		return node;
	}

	std::vector<Token> expr;

	while (tokens[pos].type != AstNodeType::Semicolon)
	{
		if (tokens[pos].type == AstNodeType::EndOfExpression)
			throw std::runtime_error("Missing ';' after return");

		expr.push_back(tokens[pos++]);
	}

	pos++;
	expr.push_back(Token("", AstNodeType::EndOfExpression));

	node->expr = parseExpression(expr, symbolTable);

	return node;
}


std::unique_ptr<AstNode> parseFunctionCall(std::vector<Token>& tokens, SymbolTable& symbolTable, int& pos)
{
	if (tokens[pos].type != AstNodeType::Variable)
		throw std::runtime_error("Expected function name");

	auto node = std::make_unique<CallExpressionNode>();
	node->name = tokens[pos].value;

	pos++;

	if (tokens[pos].type != AstNodeType::OpenParen)
		throw std::runtime_error("Expected (");

	pos++;

	while (tokens[pos].type != AstNodeType::CloseParen)
	{
		std::vector<Token> expr;

		int depth = 0;

		while (!(tokens[pos].type == AstNodeType::Comma && depth == 0) &&
				!(tokens[pos].type == AstNodeType::CloseParen && depth == 0))
		{
			if (tokens[pos].type == AstNodeType::OpenParen) depth++;
			if (tokens[pos].type == AstNodeType::CloseParen) depth--;

			expr.push_back(tokens[pos++]);
		}

		expr.push_back(Token("", AstNodeType::EndOfExpression));

		node->args.push_back(parseExpression(expr, symbolTable));

		if (tokens[pos].type == AstNodeType::Comma)
			pos++;
	}

	pos++;

	if (tokens[pos].type == AstNodeType::Semicolon)
		pos++;

	return node;
}


std::unique_ptr<AstNode> parseStatement(std::vector<Token>& tokens, SymbolTable& symbolTable, int& pos)
{
	if (static_cast<size_t>(pos) >= tokens.size())
		throw std::runtime_error("Unexpected end of input");

	switch (tokens[pos].type)
	{
		case AstNodeType::If:
			return parseIfStatement(tokens, symbolTable, pos);
		case AstNodeType::TypeKeyword:
			return parseVariableDeclaration(tokens, symbolTable, pos);
		case AstNodeType::Variable:
		{
			if (static_cast<size_t>(pos + 1) < tokens.size() && tokens[pos + 1].type == AstNodeType::OpenParen)
				return parseFunctionCall(tokens, symbolTable, pos);

			if (static_cast<size_t>(pos + 1) < tokens.size() && tokens[pos + 1].type == AstNodeType::Assign)
				return parseAssignment(tokens, symbolTable, pos);
			break;
		}
		case AstNodeType::OpenBrace:
			return parseBlock(tokens, symbolTable, pos);
		case AstNodeType::While:
			return parseWhileLoop(tokens, symbolTable, pos);
		case AstNodeType::ReturnStatement:
		{
			if (!isInsideFunction)
				throw std::runtime_error("return outside of function");
			return parseReturnStatement(tokens, symbolTable, pos);
		}
		default:
			break;
	}
	std::vector<Token> expr;
	while (static_cast<size_t>(pos) < tokens.size() && tokens[pos].type != AstNodeType::Semicolon)
		expr.push_back(tokens[pos++]);

	if (static_cast<size_t>(pos) >= tokens.size())
		throw std::runtime_error("Missing ';'");
	expr.push_back(Token("", AstNodeType::EndOfExpression));
	pos++;
	return parseExpression(expr, symbolTable);
}

std::unique_ptr<AstNode> parseBlock(std::vector<Token>& tokens, SymbolTable& symbolTable, int& pos)
{
	auto block = std::make_unique<BlockStatementNode>();

	if (tokens[pos].type != AstNodeType::OpenBrace)
		throw std::runtime_error("Expected { " + tokens[pos].value);

	pos++;

	symbolTable.enterScope();

	while (tokens[pos].type != AstNodeType::CloseBrace)
	{
		if (tokens[pos].type == AstNodeType::EndOfExpression)
			throw std::runtime_error("Expected } " + tokens[pos].value);

		auto stmt = parseStatement(tokens, symbolTable, pos);

		if (stmt)
			block->statements.push_back(std::move(stmt));
	}

	symbolTable.exitScope();

	pos++;

	return block;
}


std::unique_ptr<AstNode> parseFunction(std::vector<Token>& tokens, SymbolTable& symbolTable, int& pos)
{
	if (tokens[pos].type != AstNodeType::FunctionDecl)
		throw std::runtime_error("Expected keyword \"Func\" " + tokens[pos].value);

	pos++;
	if (tokens[pos].type != AstNodeType::TypeKeyword)
		throw std::runtime_error("Expected a type " + tokens[pos].value);
	
	auto func = std::make_unique<FunctionNode>();
	if (tokens[pos].value == "int")
		func->returnType = ReturnType::Int;
	else if (tokens[pos].value == "void")
		func->returnType = ReturnType::Void;
	else
		throw std::runtime_error("Expected a type " + tokens[pos].value);

	pos++;

	if (tokens[pos].type != AstNodeType::Variable)
		throw std::runtime_error("Expected function name " + tokens[pos].value);

	func->name = tokens[pos].value;

	pos++;

	if (tokens[pos].type != AstNodeType::OpenParen)
		throw std::runtime_error("Expected ( "  + tokens[pos].value);

	pos++;

	while (tokens[pos].type != AstNodeType::CloseParen)
	{
		if (tokens[pos].type != AstNodeType::TypeKeyword)
			throw std::runtime_error("Expected parameter type " + tokens[pos].value);

		std::string type = tokens[pos].value;
		pos++;

		if (tokens[pos].type != AstNodeType::Variable)
			throw std::runtime_error("Expected parameter name " + tokens[pos].value);

		std::string name = tokens[pos].value;
		pos++;

		func->params.push_back({type, name});

		if (tokens[pos].type == AstNodeType::Comma)
			pos++;
		else if (tokens[pos].type != AstNodeType::CloseParen)
			throw std::runtime_error("Expected ',' or ')' " + tokens[pos].value);

	}

	pos++;

	symbolTable.enterScope();
	for (const auto& p : func->params)
	{
		if (symbolTable.isDeclared(p.name))
			throw std::runtime_error("Duplicate parameter: " + p.name);

		symbolTable.declareVariable(p.name);
	}

	if (tokens[pos].type != AstNodeType::OpenBrace)
		throw std::runtime_error("Expected { for function body " + tokens[pos].value);
	
	isInsideFunction = 1;
	func->body = std::unique_ptr<BlockStatementNode>(
		static_cast<BlockStatementNode*>(parseBlock(tokens, symbolTable, pos).release())
	);
	isInsideFunction = 0;
	symbolTable.exitScope();
	for (auto& stmt : func->body->statements)
	{
		if (stmt->type == AstNodeType::ReturnStatement)
		{
			func->returnStatementNode = static_cast<ReturnStatementNode*>(stmt.get());
			break;
		}
	}
	return func;
}

std::vector<std::unique_ptr<AstNode>> parser(std::vector<Token>& tokens, SymbolTable& symbolTable, int& pos)
{
	std::vector<std::unique_ptr<AstNode>> functionEntryPoints;

	std::unordered_set<std::string> functionNames;

	while (tokens[pos].type != AstNodeType::EndOfExpression)
	{
		int oldPos = pos;

		if (tokens[pos].type != AstNodeType::FunctionDecl)
			throw std::runtime_error("Only functionEntryPoints allowed at global scope");

		auto func = parseFunction(tokens, symbolTable, pos);

		FunctionNode* fn = dynamic_cast<FunctionNode*>(func.get());

		if (!fn)
			throw std::runtime_error("Internal parser error");

		if (functionNames.find(fn->name) != functionNames.end())
			throw std::runtime_error("Multiple definition of function: " + fn->name);

		if (fn->returnType == ReturnType::Void && fn->returnStatementNode != nullptr &&
				fn->returnStatementNode->expr != nullptr)
			throw std::runtime_error("Void function cannot return a value");
	
		if (fn->returnType == ReturnType::Int && fn->returnStatementNode == nullptr)
			throw std::runtime_error("Function should return a value");

		functionNames.insert(fn->name);

		functionEntryPoints.push_back(std::move(func));

		if (pos == oldPos)
			throw std::runtime_error("Parser stuck (no progress)");
	}

	return functionEntryPoints;
}