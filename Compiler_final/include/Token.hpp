#pragma once

#include <iostream>
#include "ast-node-type.hpp"
#include <vector>


struct Token
{
	std::string value;
	AstNodeType type;

	Token(const std::string& _value, const AstNodeType _type) : value(_value), type(_type) { };
	~Token() { };
};

std::vector<Token> tokenizer(const std::vector<std::string> &words);
