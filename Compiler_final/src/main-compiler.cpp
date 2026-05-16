#include "../include/compiler-pipeline.hpp"

void printTokenDebug(const Token& node)
{
	switch (node.type)
	{
		case AstNodeType::Number:
			std::cout << "Type: Number, Value: " << node.value << "\n";
			break;
		case AstNodeType::Variable:
			std::cout << "Type: Variable, Name: " << node.value << "\n";
			break;
		case AstNodeType::BinaryOperator:
			std::cout << "Type: ArithmeticOperator: " << node.value << "\n";
			break;
		case AstNodeType::OpenParen:
			std::cout << "Type: Open Bracket, Symbol: " << node.value << "\n";
			break;
		case AstNodeType::CloseParen:
			std::cout << "Type: Close Bracket, Symbol: " << node.value << "\n";
			break;
		case AstNodeType::EndOfExpression:
			std::cout << "Type: End of Expression\n";
			break;
		case AstNodeType::If:
			std::cout << "Type: If\n";
			break;
		case AstNodeType::While:
			std::cout << "Type: While\n";
			break;
		case AstNodeType::Comparison:
			std::cout << "Type: Comparison, Symbol: " << node.value << "\n";
			break;
		case AstNodeType::Assign:
			std::cout << "Type: Assignment, Symbol: " << node.value << "\n";
			break;
		case AstNodeType::Not:
			std::cout << "Type: Not, Symbol: " << node.value << "\n";
			break;
		case AstNodeType::Semicolon:
			std::cout << "Type: Semicolon, Symbol: " << node.value << "\n";
			break;
		case AstNodeType::OpenBrace:
			std::cout << "Type: Open Curly Bracket, Symbol: " << node.value << "\n";
			break;
		case AstNodeType::CloseBrace:
			std::cout << "Type: Close Curly Bracket, Symbol: " << node.value << "\n";
			break;
		case AstNodeType::TypeKeyword:
			std::cout << "Type: Type, Symbol: " << node.value << "\n";
			break;
		case AstNodeType::ReturnStatement:
			std::cout << "Type: Return, Symbol: " << node.value << "\n";
			break;
		default:
			std::cout << "Type: Unknown\n";
			break;
	}
}

void printAstDebug(AstNode* node)
{
	if (!node)
		return;


	switch (node->type)
	{
		case AstNodeType::Number:
			std::cout << "Num(" << node->value << ")\n";
			return;
		case AstNodeType::Variable:
			std::cout << "Var(addr=" << node->symbolAddress << ")\n";
			return;
		case AstNodeType::Assign:
			std::cout << "Assign\n";
			break;
		case AstNodeType::BinaryOperator:
		{
			switch (node->op)
			{
			case ArithmeticOperator::Addition:
				std::cout << "Add\n";
				break;
			case ArithmeticOperator::Subtraction:
				std::cout << "Sub\n";
				break;
			case ArithmeticOperator::Multiplication:
				std::cout << "Mult\n";
				break;
			case ArithmeticOperator::Division:
				std::cout << "Div\n";
				break;
			default:
				break;
			}
			break;
		}
		case AstNodeType::BlockStatement:
		{
			std::cout << "Block\n";
			BlockStatementNode* n = dynamic_cast<BlockStatementNode*>(node);
			for (size_t i = 0; i < n->statements.size(); i++)
			{
				printAstDebug(n->statements[i].get());
			}
			return;
		}
		case AstNodeType::FunctionDecl:
		{
			FunctionNode* n = dynamic_cast<FunctionNode*>(node);

			std::cout<<"Func: "<<n->name<<" with params\n";
			for (size_t i = 0; i < n->params.size(); i++)
			{
				std::cout<<n->params[i].type<<"  "<<n->params[i].name<<"\n";
			}
			printAstDebug(n->body.get());
			break;
		}
		case AstNodeType::ReturnStatement:
		{
			ReturnStatementNode* n = dynamic_cast<ReturnStatementNode*>(node);
			std::cout << "Return\n";
			printAstDebug(n->expr.get());
			break;
		}
		case AstNodeType::FunctionCall:
		{
			CallExpressionNode* n = dynamic_cast<CallExpressionNode*>(node);
			std::cout << "Call " << n->name << " with params\n";
			for (size_t i = 0; i < n->args.size(); i++)
			{
				std::cout<<n->args[i].get()->name<<"\n";
			}
			break;
		}
		default:
			std::cout << "AstNodeType(" << (int)node->type << ")\n";
			break;
	}

	if (node->left)
	{
		std::cout << "L:";
		printAstDebug(node->left.get());
	}

	if (node->right)
	{
		std::cout << "R:";
		printAstDebug(node->right.get());
	}
}



int main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cerr << "Error: Expecting a file with code\n";
		return 1;
	}
	std::ifstream file(argv[1]);
	if (!file)
	{
		std::cerr << "Error: Cannot open file\n";
		return 1;
	}

	std::stringstream buffer;
	buffer << file.rdbuf();

	std::string expression = buffer.str();

	SymbolTable symbolTable;

	try
	{
		std::stringstream line(expression);

		int pos = 0;
		std::vector<std::string> v = lexer(line);
		std::vector<Token> tokens = tokenizer(v);
		std::vector<std::unique_ptr<AstNode>> trees = parser(tokens, symbolTable, pos);

		// for(auto& c:tokens)
		// 	printTokenDebug(c);
		VirtualMachine vm;
		for(auto& c:trees){
			// printAstDebug(c.get());
			// std::cout <<"\nNEXT FUNC\n";
			vm.compile(c.get());
			vm.writeExecutableFile();
		}
		// vm.visualizeProgram();

		// std::cout << "\nValue: " << vm.execute(ST) << "\n";
	}
	catch(std::exception& e)
	{
		std::cout << e.what() << '\n';
	}
}
