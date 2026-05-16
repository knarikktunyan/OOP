#include "../include/virtual-machine.hpp"
#include <sstream>

VirtualMachine::VirtualMachine() : registers(100), nextRegisterIndex(0), comparisonFlag(0) { }

VirtualMachine::~VirtualMachine() { }

int VirtualMachine::compileNumberLiteral(AstNode* node)
{
	int dest = nextRegisterIndex++;

	uint8_t cIdx = static_cast<uint8_t>(constants.size());
	constants.push_back(node->value);

	program.push_back({
		static_cast<uint8_t>(InstructionOpcode::LOAD_NUMBER),
		static_cast<uint8_t>(dest),
		cIdx,
		0
	});

	return dest;
}

int VirtualMachine::compileVariableLoad(AstNode* node)
{
	int dest = nextRegisterIndex++;

	program.push_back({
		static_cast<uint8_t>(InstructionOpcode::LOAD_VARIABLE),
		static_cast<uint8_t>(dest),
		static_cast<uint8_t>(node->symbolAddress),
		0
	});

	return dest;
}

int VirtualMachine::compileBinaryOperation(AstNode* node)
{
	int l = compile(node->left.get());
	int r = compile(node->right.get());

	int dest = nextRegisterIndex++;

	InstructionOpcode op;
	switch (node->op)
	{
		case ArithmeticOperator::Addition:  op = InstructionOpcode::ADD; break;
		case ArithmeticOperator::Subtraction:  op = InstructionOpcode::SUB; break;
		case ArithmeticOperator::Multiplication: op = InstructionOpcode::MUL; break;
		case ArithmeticOperator::Division:  op = InstructionOpcode::DIV; break;
		default:
			throw std::runtime_error("Unknown operator");
	}

	program.push_back({
		static_cast<uint8_t>(op),
		static_cast<uint8_t>(dest),
		static_cast<uint8_t>(l),
		static_cast<uint8_t>(r)
	});

	return dest;
}

int VirtualMachine::compileAssignment(AstNode* node)
{
	size_t addr = node->left->symbolAddress;

	int rhs = compile(node->right.get());

	program.push_back({
		static_cast<uint8_t>(InstructionOpcode::STORE_VARIABLE),
		static_cast<uint8_t>(rhs),
		static_cast<uint8_t>(addr),
		0
	});

	return rhs;
}

int VirtualMachine::compileBlock(AstNode* node)
{
	const BlockStatementNode* bnode = static_cast<const BlockStatementNode*>(node);
	for (auto& stmt : bnode->statements)
		compile(stmt.get());

	return -1;
}

int VirtualMachine::compileIfStatement(AstNode* node)
{
	IfStatementNode* n = dynamic_cast<IfStatementNode*>(node);

	int jmpIndex = compileComparison(n->condition.get());

	compile(n->trueBranch.get());

	if (n->falseBranch)
	{
		int jmpEnd = program.size();
		program.push_back({
			static_cast<uint8_t>(InstructionOpcode::JMP),
			0, 0, 0
		});

		program[jmpIndex].dest = static_cast<uint8_t>(program.size());

		compile(n->falseBranch.get());

		program[jmpEnd].dest = static_cast<uint8_t>(program.size());
	}
	else
		program[jmpIndex].dest = static_cast<uint8_t>(program.size());

	return -1;
}

int VirtualMachine::compileWhileLoop(AstNode* node)
{
	WhileStatementNode* n = dynamic_cast<WhileStatementNode*>(node);

	int loopStart = static_cast<int>(program.size());

	int jmpExit = compileComparison(n->condition.get());

	compile(n->body.get());

	program.push_back({
		static_cast<uint8_t>(InstructionOpcode::JMP),
		static_cast<uint8_t>(loopStart),
		0, 0
	});

	program[jmpExit].dest = static_cast<uint8_t>(program.size());

	return -1;
}

int VirtualMachine::compileComparison(AstNode* node)
{
	int l = compile(node->left.get());
	int r = compile(node->right.get());

	program.push_back({
		static_cast<uint8_t>(InstructionOpcode::CMP),
		0,
		static_cast<uint8_t>(l),
		static_cast<uint8_t>(r)
	});

	InstructionOpcode jmp;
	if      (node->name == "==") jmp = InstructionOpcode::JNE;
	else if (node->name == "!=") jmp = InstructionOpcode::JE;
	else if (node->name == ">=") jmp = InstructionOpcode::JL;
	else if (node->name == "<=") jmp = InstructionOpcode::JG;
	else if (node->name == ">")  jmp = InstructionOpcode::JLE;
	else if (node->name == "<")  jmp = InstructionOpcode::JGE;
	else
		throw std::runtime_error("Unknown comparison operator: " + node->name);

	int jmpIndex = static_cast<int>(program.size());

	program.push_back({
		static_cast<uint8_t>(jmp),
		0, 0, 0
	});

	return jmpIndex;
}

void VirtualMachine::compileFunctionDefinition(AstNode* node)
{
	FunctionNode* fn = static_cast<FunctionNode*>(node);
	functionEntryPoints[fn->name] = static_cast<int>(program.size());

	int skipJmp = static_cast<int>(program.size());
	program.push_back({
		static_cast<uint8_t>(InstructionOpcode::JMP),
		0, 0, 0
	});

	int savedNext = nextRegisterIndex;
	nextRegisterIndex = 0;

	compile(fn->body.get());

	if (program.empty() ||
		static_cast<InstructionOpcode>(program.back().op) != InstructionOpcode::RET)
	{
		program.push_back({
			static_cast<uint8_t>(InstructionOpcode::RET),
			0, 0, 0
		});
	}

	nextRegisterIndex = savedNext;

	program[skipJmp].dest = static_cast<uint8_t>(program.size());
}

int VirtualMachine::compileFunctionCall(AstNode* node)
{
	CallExpressionNode* cnode = static_cast<CallExpressionNode*>(node);

	for (auto& arg : cnode->args)
		compile(arg.get());

	auto it = functionEntryPoints.find(cnode->name);
	if (it == functionEntryPoints.end())
		throw std::runtime_error("Undefined function: " + cnode->name);

	int dest = nextRegisterIndex++;

	program.push_back({
		static_cast<uint8_t>(InstructionOpcode::CALL),
		static_cast<uint8_t>(dest),
		static_cast<uint8_t>(it->second),
		0
	});

	return dest;
}

int VirtualMachine::compileReturnStatement(AstNode* node)
{
	ReturnStatementNode* ret = static_cast<ReturnStatementNode*>(node);

	if (ret->expr)
	{
		int reg = compile(ret->expr.get());

		program.push_back({
			static_cast<uint8_t>(InstructionOpcode::RET),
			static_cast<uint8_t>(reg),
			0, 0
		});

		return reg;
	}

	program.push_back({
		static_cast<uint8_t>(InstructionOpcode::RET),
		0, 0, 0
	});

	return -1;
}

int VirtualMachine::compile(AstNode* node)
{
	if (!node)
		return -1;

	switch (node->type)
	{
		case AstNodeType::Number:    return compileNumberLiteral(node);
		case AstNodeType::Variable:    return compileVariableLoad(node);
		case AstNodeType::BinaryOperator:     return compileBinaryOperation(node);
		case AstNodeType::Assign: return compileAssignment(node);
		case AstNodeType::BlockStatement:  return compileBlock(node);
		case AstNodeType::If:     return compileIfStatement(node);
		case AstNodeType::Comparison:   return compileComparison(node);
		case AstNodeType::While:  return compileWhileLoop(node);
		case AstNodeType::ReturnStatement:    return compileReturnStatement(node);
		case AstNodeType::FunctionDecl:
			compileFunctionDefinition(static_cast<FunctionNode*>(node));
			return -1;
		case AstNodeType::FunctionCall:
			return compileFunctionCall(static_cast<CallExpressionNode*>(node));
		default:
			throw std::runtime_error("Unknown node type in compile: " +
			                         std::to_string(static_cast<int>(node->type)));
	}
}

//  visualizeProgram()  –  prints the instruction list to stdout

void VirtualMachine::visualizeProgram() const
{
	std::cout << "\n[VirtualMachine Assembly]\n";
	std::cout << "----------------------\n";

	for (size_t i = 0; i < program.size(); i++)
	{
		const auto& inst = program[i];
		std::cout << std::setw(3) << i << ": ";

		switch (static_cast<InstructionOpcode>(inst.op))
		{
			case InstructionOpcode::LOAD_NUMBER:
				std::cout << "MOV r" << static_cast<int>(inst.dest)
				          << ", #" << constants[inst.left];
				break;
			case InstructionOpcode::LOAD_VARIABLE:
				std::cout << "MOV r" << static_cast<int>(inst.dest)
				          << ", [" << static_cast<int>(inst.left) << "]";
				break;
			case InstructionOpcode::STORE_VARIABLE:
				std::cout << "MOV [" << static_cast<int>(inst.left)
				          << "], r" << static_cast<int>(inst.dest);
				break;
			case InstructionOpcode::ADD:
				std::cout << "ADD r" << static_cast<int>(inst.dest)
				          << ", r" << static_cast<int>(inst.left)
				          << ", r" << static_cast<int>(inst.right);
				break;
			case InstructionOpcode::SUB:
				std::cout << "SUB r" << static_cast<int>(inst.dest)
				          << ", r" << static_cast<int>(inst.left)
				          << ", r" << static_cast<int>(inst.right);
				break;
			case InstructionOpcode::MUL:
				std::cout << "MUL r" << static_cast<int>(inst.dest)
				          << ", r" << static_cast<int>(inst.left)
				          << ", r" << static_cast<int>(inst.right);
				break;
			case InstructionOpcode::DIV:
				std::cout << "DIV r" << static_cast<int>(inst.dest)
				          << ", r" << static_cast<int>(inst.left)
				          << ", r" << static_cast<int>(inst.right);
				break;
			case InstructionOpcode::CMP:
				std::cout << "CMP r" << static_cast<int>(inst.left)
				          << ", r" << static_cast<int>(inst.right);
				break;
			case InstructionOpcode::JMP:
				std::cout << "JMP " << static_cast<int>(inst.dest);
				break;
			case InstructionOpcode::JE:
				std::cout << "JE "  << static_cast<int>(inst.dest);
				break;
			case InstructionOpcode::JNE:
				std::cout << "JNE " << static_cast<int>(inst.dest);
				break;
			case InstructionOpcode::JG:
				std::cout << "JG "  << static_cast<int>(inst.dest);
				break;
			case InstructionOpcode::JL:
				std::cout << "JL "  << static_cast<int>(inst.dest);
				break;
			case InstructionOpcode::JGE:
				std::cout << "JGE " << static_cast<int>(inst.dest);
				break;
			case InstructionOpcode::JLE:
				std::cout << "JLE " << static_cast<int>(inst.dest);
				break;
			case InstructionOpcode::CALL:
				std::cout << "CALL " << static_cast<int>(inst.left);
				break;
			case InstructionOpcode::RET:
				std::cout << "RET";
				if (inst.dest != 0)
					std::cout << " r" << static_cast<int>(inst.dest);
				break;
		}

		std::cout << "\n";
	}
}


//  writeExecutableFile()  –  writes the instruction list as text to ./exe


void VirtualMachine::writeExecutableFile()
{
	std::fstream exe;
	exe.open("./exe", std::ios::out | std::ios::trunc);
	if (!exe.is_open())
		throw std::runtime_error("Could not open ./exe for writing");

	// ── Constant pool section ────────────────────────────────────────────────
	// Written first so the VirtualMachine loader can rebuild the pool before parsing code.
	// Format:  .CONST <count>
	//          <value0>
	//          <value1>  ...
	exe << ".CONST " << constants.size() << "\n";
	for (int c : constants)
		exe << c << "\n";

	// ── Function address table ───────────────────────────────────────────────
	// Format:  .FUNC <name> <address>
	for (const auto& [name, addr] : functionEntryPoints)
		exe << ".FUNC " << name << " " << addr << "\n";

	// ── Code section ────────────────────────────────────────────────────────
	exe << ".CODE\n";

	for (size_t i = 0; i < program.size(); i++)
	{
		const auto& inst = program[i];

		switch (static_cast<InstructionOpcode>(inst.op))
		{
			case InstructionOpcode::LOAD_NUMBER:
				exe << "MOV r" << static_cast<int>(inst.dest)
				    << ", #" << constants[inst.left];
				break;

			case InstructionOpcode::LOAD_VARIABLE:
				exe << "MOV r" << static_cast<int>(inst.dest)
				    << ", [" << static_cast<int>(inst.left) << "]";
				break;

			case InstructionOpcode::STORE_VARIABLE:
				exe << "MOV [" << static_cast<int>(inst.left)
				    << "], r" << static_cast<int>(inst.dest);
				break;

			case InstructionOpcode::ADD:
				exe << "ADD r" << static_cast<int>(inst.dest)
				    << ", r" << static_cast<int>(inst.left)
				    << ", r" << static_cast<int>(inst.right);
				break;

			case InstructionOpcode::SUB:
				exe << "SUB r" << static_cast<int>(inst.dest)
				    << ", r" << static_cast<int>(inst.left)
				    << ", r" << static_cast<int>(inst.right);
				break;

			case InstructionOpcode::MUL:
				exe << "MUL r" << static_cast<int>(inst.dest)
				    << ", r" << static_cast<int>(inst.left)
				    << ", r" << static_cast<int>(inst.right);
				break;

			case InstructionOpcode::DIV:
				exe << "DIV r" << static_cast<int>(inst.dest)
				    << ", r" << static_cast<int>(inst.left)
				    << ", r" << static_cast<int>(inst.right);
				break;

			case InstructionOpcode::CMP:
				exe << "CMP r" << static_cast<int>(inst.left)
				    << ", r" << static_cast<int>(inst.right);
				break;

			case InstructionOpcode::JMP:
				exe << "JMP " << static_cast<int>(inst.dest);
				break;

			case InstructionOpcode::JE:  exe << "JE "  << static_cast<int>(inst.dest); break;
			case InstructionOpcode::JNE: exe << "JNE " << static_cast<int>(inst.dest); break;
			case InstructionOpcode::JG:  exe << "JG "  << static_cast<int>(inst.dest); break;
			case InstructionOpcode::JL:  exe << "JL "  << static_cast<int>(inst.dest); break;
			case InstructionOpcode::JGE: exe << "JGE " << static_cast<int>(inst.dest); break;
			case InstructionOpcode::JLE: exe << "JLE " << static_cast<int>(inst.dest); break;

			case InstructionOpcode::CALL:
				exe << "CALL " << static_cast<int>(inst.left);
				break;

			case InstructionOpcode::RET:
				exe << "RET";
				if (inst.dest != 0)
					exe << " r" << static_cast<int>(inst.dest);
				break;
		}

		exe << "\n";
	}

	exe.close();
}

void VirtualMachine::loadExecutableFile(const std::string& filename)
{
	std::ifstream file(filename);
	if (!file.is_open())
		throw std::runtime_error("Cannot open file: " + filename);

	std::string line;
	bool inCode = false;

	while (std::getline(file, line))
	{
		if (line.empty()) continue;

		if (line.rfind(".CONST", 0) == 0) {
			int count = std::stoi(line.substr(7));
			for (int i = 0; i < count; i++) {
				std::getline(file, line);
				constants.push_back(std::stoi(line));
			}
			continue;
		}
		if (line.rfind(".FUNC", 0) == 0) {
			std::istringstream ss(line.substr(6));
			std::string name; int addr;
			ss >> name >> addr;
			functionEntryPoints[name] = addr;
			continue;
		}
		if (line == ".CODE") { inCode = true; continue; }

		if (inCode)
			parseLine(line);
	}

	file.close();
}

int VirtualMachine::regIndex(const std::string& r)
{
	return std::stoi(r.substr(1));
}

Operand VirtualMachine::defType(const std::string& var) const
{
	if (!var.empty() && var[0] == 'r')
		return Operand::REG;
	if (!var.empty() && var[0] == '[')
		return Operand::MEM;
	return Operand::CONST;
}

void VirtualMachine::parseLine(const std::string& line)
{
	std::istringstream ss(line);
	std::string op;
	ss >> op;

	auto stripComma = [](std::string s) -> std::string {
		if (!s.empty() && s.back() == ',') s.pop_back();
		return s;
	};

	if (op == "MOV")
	{
		std::string dst, src;
		ss >> dst >> src;
		dst = stripComma(dst);

		if (defType(dst) == Operand::REG)
		{
			uint8_t D = static_cast<uint8_t>(regIndex(dst));
			if (src[0] == '#')
			{
				int val = std::stoi(src.substr(1));
				uint8_t cIdx = static_cast<uint8_t>(constants.size());
				constants.push_back(val);
				program.push_back({ static_cast<uint8_t>(InstructionOpcode::LOAD_NUMBER), D, cIdx, 0 });
			}
			else if (defType(src) == Operand::MEM)
			{
				std::string addr = src.substr(1, src.size() - 2);
				uint8_t A = static_cast<uint8_t>(std::stoi(addr));
				program.push_back({ static_cast<uint8_t>(InstructionOpcode::LOAD_VARIABLE), D, A, 0 });
			}
		}
		else if (defType(dst) == Operand::MEM)
		{
			std::string addrStr = dst.substr(1, dst.size() - 2);
			uint8_t A = static_cast<uint8_t>(std::stoi(addrStr));
			uint8_t S = static_cast<uint8_t>(regIndex(src));
			program.push_back({ static_cast<uint8_t>(InstructionOpcode::STORE_VARIABLE), S, A, 0 });
		}
		return;
	}

	auto parseArith = [&](InstructionOpcode aop) {
		std::string rD, rL, rR;
		ss >> rD >> rL >> rR;
		uint8_t D = static_cast<uint8_t>(regIndex(stripComma(rD)));
		uint8_t L = static_cast<uint8_t>(regIndex(stripComma(rL)));
		uint8_t R = static_cast<uint8_t>(regIndex(stripComma(rR)));
		program.push_back({ static_cast<uint8_t>(aop), D, L, R });
	};

	if      (op == "ADD") { parseArith(InstructionOpcode::ADD); return; }
	else if (op == "SUB") { parseArith(InstructionOpcode::SUB); return; }
	else if (op == "MUL") { parseArith(InstructionOpcode::MUL); return; }
	else if (op == "DIV") { parseArith(InstructionOpcode::DIV); return; }

	if (op == "CMP")
	{
		std::string rL, rR;
		ss >> rL >> rR;
		uint8_t L = static_cast<uint8_t>(regIndex(stripComma(rL)));
		uint8_t R = static_cast<uint8_t>(regIndex(rR));
		program.push_back({ static_cast<uint8_t>(InstructionOpcode::CMP), 0, L, R });
		return;
	}

	auto parseJump = [&](InstructionOpcode jop) {
		std::string addr;
		ss >> addr;
		uint8_t target = static_cast<uint8_t>(std::stoi(addr));
		program.push_back({ static_cast<uint8_t>(jop), target, 0, 0 });
	};

	if      (op == "JMP") { parseJump(InstructionOpcode::JMP); return; }
	else if (op == "JE")  { parseJump(InstructionOpcode::JE);  return; }
	else if (op == "JNE") { parseJump(InstructionOpcode::JNE); return; }
	else if (op == "JG")  { parseJump(InstructionOpcode::JG);  return; }
	else if (op == "JL")  { parseJump(InstructionOpcode::JL);  return; }
	else if (op == "JGE") { parseJump(InstructionOpcode::JGE); return; }
	else if (op == "JLE") { parseJump(InstructionOpcode::JLE); return; }

	if (op == "CALL")
	{
		std::string addr;
		ss >> addr;
		uint8_t target = static_cast<uint8_t>(std::stoi(addr));
		program.push_back({ static_cast<uint8_t>(InstructionOpcode::CALL), 0, target, 0 });
		return;
	}

	if (op == "RET")
	{
		std::string maybeReg;
		uint8_t retReg = 0;
		if (ss >> maybeReg)
			retReg = static_cast<uint8_t>(regIndex(maybeReg));
		program.push_back({ static_cast<uint8_t>(InstructionOpcode::RET), retReg, 0, 0 });
		return;
	}
}







