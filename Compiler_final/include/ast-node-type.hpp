#pragma once

enum class ArithmeticOperator
{
	Addition,
	Subtraction,
	Division,
	Multiplication,
};

enum class AstNodeType
{
	Number,
	Variable,
	EndOfExpression,
	OpenParen,
	CloseParen,
	BinaryOperator,
	If,
	While,
	Else,
	Comparison,
	Assign,
	Not,
	Semicolon,
	OpenBrace,
	CloseBrace,
	TypeKeyword,
	BlockStatement,
	ReturnStatement,
	FunctionDecl,
	FunctionCall,
	Comma,
};
