#pragma once

#include <iostream>
#include <map>
#include <string>

enum NodeType
{
	ADD,
	SUB,
	DIV,
	MUL,
	VAL,
	VAR
};

struct Node
{
	int value;
	NodeType type;
	Node* left;
	Node* right;
	std::string var;

	Node(int v, NodeType t, Node* l, Node* r, const std::string& name)
		: value(v), type(t), left(l), right(r), var(name) {}

	Node(int v, NodeType t, Node* l, Node* r)
		: value(v), type(t), left(l), right(r) {}

	Node(int v) : value(v), type(VAL), left(nullptr), right(nullptr) {}

	Node(const std::string& name)
		: value(0), type(VAR), left(nullptr), right(nullptr), var(name) {}
};

Node *parse_expression(char *input);
int evaluate(const Node *root, const std::map<std::string, int>& context);
void free_tree(Node *root);
void parse_assignment(char* str, std::map<std::string, int>& context);
