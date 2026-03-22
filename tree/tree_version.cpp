#include "Node1.hpp"

void free_tree(Node *root)
{
	if (!root)
		return;
	free_tree(root->left);
	free_tree(root->right);
	delete root;
}

void report_error(char c)
{
	if (c)
		std::cout << "Unexpected token: " << c << "\n";
	else
		std::cout << "Unexpected end of input\n";
}

int match_char(char **input, char expected)
{
	if (**input == expected)
	{
		(*input)++;
		return 1;
	}
	return 0;
}

int require_char(char **input, char expected)
{
	if (match_char(input, expected))
		return 1;
	report_error(**input);
	return 0;
}

static Node *parse_expression_internal(char **input);
static Node *parse_term_level(char **input);
static Node *parse_primary(char **input);

static Node *parse_primary(char **input)
{
	if (isdigit((unsigned char)**input))
	{
		int num = 0;
		while (isdigit((unsigned char)**input))
		{
			num = num * 10 + (**input - '0');
			(*input)++;
		}
		return new Node(num);
	}

	if (isalpha((unsigned char)**input) || **input == '_')
	{
		std::string identifier;
		while (isalnum((unsigned char)**input))
		{
			identifier += **input;
			(*input)++;
		}
		return new Node(identifier);
	}

	if (match_char(input, '('))
	{
		Node *expr = parse_expression_internal(input);
		if (!expr)
			return nullptr;

		if (!require_char(input, ')'))
		{
			free_tree(expr);
			return nullptr;
		}
		return expr;
	}

	report_error(**input);
	return nullptr;
}

static Node *parse_term_level(char **input)
{
	Node *lhs = parse_primary(input);
	if (!lhs)
		return nullptr;

	while (true)
	{
		if (match_char(input, '*'))
		{
			Node *rhs = parse_primary(input);
			if (!rhs)
			{
				free_tree(lhs);
				return nullptr;
			}
			lhs = new Node(0, MUL, lhs, rhs);
		}
		else if (match_char(input, '/'))
		{
			Node *rhs = parse_primary(input);
			if (!rhs)
			{
				free_tree(lhs);
				return nullptr;
			}
			lhs = new Node(0, DIV, lhs, rhs);
		}
		else
			break;
	}
	return lhs;
}

static Node *parse_expression_internal(char **input)
{
	Node *lhs = parse_term_level(input);
	if (!lhs)
		return nullptr;

	while (true)
	{
		if (match_char(input, '+'))
		{
			Node *rhs = parse_term_level(input);
			if (!rhs)
			{
				free_tree(lhs);
				return nullptr;
			}
			lhs = new Node(0, ADD, lhs, rhs);
		}
		else if (match_char(input, '-'))
		{
			Node *rhs = parse_term_level(input);
			if (!rhs)
			{
				free_tree(lhs);
				return nullptr;
			}
			lhs = new Node(0, SUB, lhs, rhs);
		}
		else
			break;
	}
	return lhs;
}

Node *parse_expression(char *input)
{
	char *cursor = input;
	Node *tree = parse_expression_internal(&cursor);

	if (!tree)
		return nullptr;

	if (*cursor)
	{
		report_error(*cursor);
		free_tree(tree);
		return nullptr;
	}
	return tree;
}

int evaluate(const Node *root, const std::map<std::string, int>& context)
{
	if (!root)
		throw std::runtime_error("Empty expression tree");

	switch (root->type)
	{
	case ADD:
		return evaluate(root->left, context) + evaluate(root->right, context);
	case SUB:
		return evaluate(root->left, context) - evaluate(root->right, context);
	case MUL:
		return evaluate(root->left, context) * evaluate(root->right, context);
	case DIV:
	{
		int denom = evaluate(root->right, context);
		if (denom == 0)
			throw std::runtime_error("Division by zero");
		return evaluate(root->left, context) / denom;
	}
	case VAL:
		return root->value;
	case VAR:
	{
		auto it = context.find(root->var);
		if (it == context.end())
			throw std::runtime_error("Unknown variable: " + root->var);
		return it->second;
	}
	}
	return 0;
}

void parse_assignment(char* str, std::map<std::string, int>& context)
{
	std::string key;
	int i = 0;

	if (str[i] && (isalpha(str[i]) || str[i] == '_'))
	{
		while (str[i] && str[i] != '=')
			key += str[i++];
	}
	else
		return report_error(str[i]);

	if (!str[i])
		return report_error(str[i]);

	i++; // skip '='

	int val = 0;
	if (isdigit(str[i]))
	{
		while (isdigit(str[i]))
		{
			val = val * 10 + (str[i] - '0');
			i++;
		}
	}
	else
		return report_error(str[i]);

	context[key] = val;
}
