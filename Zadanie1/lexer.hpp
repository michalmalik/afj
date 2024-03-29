#pragma once

#include <string>
#include <vector>
#include <sstream>


class Lexer
{
private:
	std::string m_line;
	std::istringstream m_ss;

	std::string m_tokenstr;
	int m_tokennum;

	static const std::vector<std::string> token_strings;

public:
	enum Token
	{
		// 0 operators
		NOP,
		// 1 operator
		JUMP, READ, WRITE,
		// 2 operators
		ASSIGN, JUMPT, JUMPF,
		// 3 operators
		ADD, SUB, MULTIPLY,
		LT, GT, LTE, GTE, EQ,

		NUMBER, VARIABLE, STRING, EOL
	};

	explicit Lexer(std::string line);
	Token next();

	int num() const { return m_tokennum; }
	const std::string &str() const { return m_tokenstr; }
	const std::string &token_to_str(Token t) const { return token_strings.at(t); }

	// Only used for tests
	std::vector<Token> tokenize();
};