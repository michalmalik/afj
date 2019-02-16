#pragma once

#include <string>
#include <vector>
#include <sstream>


class Parser
{
private:
	std::string m_line;
	std::istringstream m_ss;

	std::string m_tokenstr;
	int m_tokennum;

public:
	enum Token
	{
		COMMA,

		// 0 operators
		NOP,
		// 1 operator
		JUMP, READ, WRITE,
		// 2 operators
		ASSIGN, JUMPT, JUMPF,
		// 3 operators
		ADD, SUB, MULTIPLY,
		LT, GT, LTE, GTE, EQ,

		NUMBER, VARIABLE, EOL
	};

	static const std::vector<std::string> token_strings;

	explicit Parser(std::string line);
	Token next();
	bool expect(Token t);
};