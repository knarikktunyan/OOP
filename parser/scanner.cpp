#include "Token.hpp"
#include <vector>
#include <sstream>
#include <cctype>
#include <stdexcept>

static void readNumber(std::stringstream& ss, std::vector<std::string>& out)
{
	std::string s;
	while (std::isdigit(ss.peek()))
		s += ss.get();
	out.push_back(s);
}

static void readWord(std::stringstream& ss, std::vector<std::string>& out)
{
	std::string s;
	while (std::isalnum(ss.peek()))
		s += ss.get();
	out.push_back(s);
}

std::vector<std::string> splitInput(std::stringstream& ss)
{
	std::vector<std::string> words;
	char c;

	while (ss.get(c))
	{
		if (std::isspace(c)) continue;

		if (std::isdigit(c))
		{
			ss.putback(c);
			readNumber(ss, words);
		}
		else if (std::isalpha(c))
		{
			ss.putback(c);
			readWord(ss, words);
		}
		else if (c=='+' || c=='-' || c=='*' || c=='/' || c=='(' || c==')')
			words.push_back(std::string(1, c));
		else
			throw std::runtime_error("invalid character");
	}
	return words;
}

std::vector<Token> makeTokens(const std::vector<std::string>& words)
{
	std::vector<Token> tokens;

	for (const auto& w : words)
	{
		if (std::isdigit(w[0]))
			tokens.emplace_back(w, Num);
		else if (std::isalpha(w[0]))
			tokens.emplace_back(w, Var);
		else if (w == "(")
			tokens.emplace_back(w, OpBr);
		else if (w == ")")
			tokens.emplace_back(w, ClBr);
		else
			tokens.emplace_back(w, Op);
	}

	tokens.emplace_back("", EofEx);
	return tokens;
}