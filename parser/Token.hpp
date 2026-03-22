#pragma once
#include <string>
#include "NodeType.hpp"

struct Token
{
	std::string value;
	NodeType type;

	Token(const std::string& v, NodeType t) : value(v), type(t) {}
};