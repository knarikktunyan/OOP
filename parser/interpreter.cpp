#include "Node.hpp"
#include "State.hpp"
#include <stack>
#include <vector>
#include <stdexcept>

static State table[3][6] =
{
	{ Wait_operator, Wait_operator, Error, Wait_operand, Error, Error },
	{ Error, Error, End, Error, Wait_operator, Wait_operand },
	{ Wait_operator, Wait_operator, Error, Wait_operand, Error, Error }
};

static int getPriority(Node* n)
{
	if (n->name == "*" || n->name == "/") return 2;
	if (n->name == "+" || n->name == "-") return 1;
	return 0;
}

Node* buildTree(std::vector<Token>& tokens, std::vector<int>& values)
{
	State state = Start;

	std::stack<Node*> ops;
	std::stack<Node*> vals;

	auto it = values.begin();

	for (size_t i = 0; i < tokens.size(); i++)
	{
		Token& t = tokens[i];
		state = table[state][t.type];

		if (state == Error)
			throw std::runtime_error("syntax error");

		if (state == End)
			break;

		Node* node = new Node(t);

		if (t.type == Num)
			vals.push(node);

		else if (t.type == Var)
		{
			if (it == values.end())
				throw std::runtime_error("missing variable");

			node->ptr = &(*it++);
			vals.push(node);
		}
		else if (t.type == Op)
		{
			while (!ops.empty() && ops.top()->type != OpBr &&
				   getPriority(ops.top()) >= getPriority(node))
			{
				Node* op = ops.top(); ops.pop();

				Node* r = vals.top(); vals.pop();
				Node* l = vals.top(); vals.pop();

				op->left = l;
				op->right = r;

				vals.push(op);
			}
			ops.push(node);
		}
		else if (t.type == OpBr)
			ops.push(node);

		else if (t.type == ClBr)
		{
			while (!ops.empty() && ops.top()->type != OpBr)
			{
				Node* op = ops.top(); ops.pop();

				Node* r = vals.top(); vals.pop();
				Node* l = vals.top(); vals.pop();

				op->left = l;
				op->right = r;

				vals.push(op);
			}

			if (ops.empty())
				throw std::runtime_error("mismatched brackets");

			delete ops.top();
			ops.pop();
			delete node;
		}
	}

	while (!ops.empty())
	{
		Node* op = ops.top(); ops.pop();

		Node* r = vals.top(); vals.pop();
		Node* l = vals.top(); vals.pop();

		op->left = l;
		op->right = r;

		vals.push(op);
	}

	return vals.top();
}

int compute(const Node* n)
{
	if (!n) throw std::runtime_error("null tree");

	if (n->type == Num) return n->value;
	if (n->type == Var) return *(n->ptr);

	if (n->type == Op)
	{
		int l = compute(n->left);
		int r = compute(n->right);

		if (n->op == Add) return l + r;
		if (n->op == Sub) return l - r;
		if (n->op == Mult) return l * r;

		if (n->op == Div)
		{
			if (r == 0) throw std::runtime_error("division by zero");
			return l / r;
		}
	}
	return 0;
}