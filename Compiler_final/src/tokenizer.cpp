#include "../include/token.hpp"

bool isNumericLiteral(const std::string &word)
{
	for (size_t i = 0; i < word.length(); i++)
	{
		if (!std::isdigit(word[i]))
			return false;
	}
	return true;
}

bool isIdentifier(const std::string &word)
{
	for (size_t i = 0; i < word.length(); i++)
	{
		if (!std::isalnum(word[i]))
			return false;
	}
	if (!std::isalpha(word[0]))
		return false;
	return true;
}

bool isComparisonOperator(const std::string& word)
{
	if (word == "<" || word == ">" || word == "<=" || word == ">=" || word == "!=" || word == "==")
		return true;
	return false;
}

std::vector<Token> tokenizer(const std::vector<std::string> &words)
{
	std::vector<Token> tokens;
	for (size_t i = 0; i < words.size(); i++)
	{
		if (words[i].empty())
			continue;
		else if (words[i] == "int" || words[i] == "void")
			tokens.push_back(Token(words[i], AstNodeType::TypeKeyword));
		else if (words[i] == "return")
			tokens.push_back(Token(words[i], AstNodeType::ReturnStatement));
		else if (words[i] == "Func")
			tokens.push_back(Token(words[i], AstNodeType::FunctionDecl));
		else if (words[i] == "if")
			tokens.push_back(Token(words[i], AstNodeType::If));
		else if (words[i] == "else")
			tokens.push_back(Token(words[i], AstNodeType::Else));
		else if (words[i] == "while")
			tokens.push_back(Token(words[i], AstNodeType::While));
		else if (isNumericLiteral(words[i]))
			tokens.push_back(Token(words[i], AstNodeType::Number));
		else if (isIdentifier(words[i]))
			tokens.push_back(Token(words[i], AstNodeType::Variable));
		else if (words[i] == "(")
			tokens.push_back(Token(words[i], AstNodeType::OpenParen));
		else if (words[i] == ")")
			tokens.push_back(Token(words[i], AstNodeType::CloseParen));
		else if (words[i] == "+" || words[i] == "-" || words[i] == "*" || words[i] == "/")
			tokens.push_back(Token(words[i], AstNodeType::BinaryOperator));
		else if (isComparisonOperator(words[i]))
			tokens.push_back(Token(words[i], AstNodeType::Comparison));
		else if (words[i] == "=")
			tokens.push_back(Token(words[i], AstNodeType::Assign));
		else if (words[i] == "!")
			tokens.push_back(Token(words[i], AstNodeType::Not));
		else if (words[i] == ";")
			tokens.push_back(Token(words[i], AstNodeType::Semicolon));
		else if (words[i] == "{")
			tokens.push_back(Token(words[i], AstNodeType::OpenBrace));
		else if (words[i] == "}")
			tokens.push_back(Token(words[i], AstNodeType::CloseBrace));
		else if (words[i] == ",")
			tokens.push_back(Token(words[i], AstNodeType::Comma));
		else
			throw std::runtime_error("unexpected token " + words[i]);
	}
	tokens.push_back(Token("", AstNodeType::EndOfExpression));
	return tokens;
}