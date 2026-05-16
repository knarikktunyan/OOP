#pragma once

#include "ast-node-type.hpp"
#include <cstdint>
#include "if-node.hpp"
#include "while-node.hpp"
#include "block-node.hpp"
#include <fstream>
#include "virtual-machine.hpp"
#include "symbol-table.hpp"
#include "token.hpp"
#include "parser-state.hpp"
#include <iomanip>
#include "function-node.hpp"
#include <unordered_set>
#include "call-node.hpp"

std::vector<std::string> lexer(std::stringstream& line);
std::vector<std::unique_ptr<AstNode>> parser(std::vector<Token>& tokens, SymbolTable& symbolTable, int& pos);
std::unique_ptr<AstNode> parseBlock(std::vector<Token>& tokens, SymbolTable& symbolTable, int& pos);
std::unique_ptr<AstNode> parseStatement(std::vector<Token>& tokens, SymbolTable& symbolTable, int& pos);


inline ParserState EXPRESSION_PARSER_FSM[3][6] =
{
	// Start
	{ ParserState::WaitOperator, ParserState::WaitOperator, ParserState::Error, ParserState::WaitOperand, ParserState::Error, ParserState::Error },

	// Wait_operator
	{ ParserState::Error, ParserState::Error, ParserState::End, ParserState::Error, ParserState::WaitOperator, ParserState::WaitOperand },

	// Wait_operand
	{ ParserState::WaitOperator, ParserState::WaitOperator, ParserState::Error, ParserState::WaitOperand, ParserState::Error, ParserState::Error }
};

enum class InstructionOpcode : uint8_t
{
	LOAD_NUMBER,
	LOAD_VARIABLE,
	STORE_VARIABLE,

	ADD,
	SUB,
	MUL,
	DIV,

	JMP,
	CMP,
	JE,
	JNE,
	JG,
	JL,
	JGE,
	JLE,

	CALL,
	RET
};



struct Instruction
{
	uint8_t op;
	uint8_t dest;
	uint8_t left;
	uint8_t right;
};
