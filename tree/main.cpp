#include "Node1.hpp"

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		std::cout << "Usage: ./expr \"expression\" \"vars(optional)\"\n";
		return 1;
	}

	Node *ast = parse_expression(argv[1]);
	if (!ast)
		return 1;

	std::map<std::string, int> variables;

	for (int i = 2; i < argc; i++)
		parse_assignment(argv[i], variables);

	std::cout << "Result: " << evaluate(ast, variables) << std::endl;

	free_tree(ast);
	return 0;
}
