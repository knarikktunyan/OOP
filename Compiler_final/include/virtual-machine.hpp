#pragma once

#include "compiler-pipeline.hpp"
#include "symbol-table.hpp"

struct Instruction;

enum class Operand
{
	MEM,
	REG,
	CONST,
};

class VirtualMachine
{
private:
	std::vector<int> registers;
	int nextRegisterIndex;
	std::vector<Instruction> program;
	std::unordered_map<std::string, int> functionEntryPoints;
	std::vector<int> constants;
	int comparisonFlag;

	int compileNumberLiteral(AstNode *node);
	int compileVariableLoad(AstNode *node);
	int compileBinaryOperation(AstNode *node);
	int compileAssignment(AstNode *node);
	int compileBlock(AstNode *node);
	int compileIfStatement(AstNode *node);
	int compileComparison(AstNode *node);
	int compileWhileLoop(AstNode *node);
	void compileFunctionDefinition(AstNode *node);
	int compileReturnStatement(AstNode *node);
	int compileFunctionCall(AstNode *node);

	void parseLine(const std::string &line);
	int regIndex(const std::string &r);
	Operand defType(const std::string &var) const;

public:
	VirtualMachine();
	~VirtualMachine();
	void visualizeProgram() const;
	int compile(AstNode *node);
	void writeExecutableFile();
	void loadExecutableFile(const std::string &name);
	// int execute(SymbolTable& symbolTable);
};
