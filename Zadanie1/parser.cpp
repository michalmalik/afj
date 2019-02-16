#include "parser.hpp"

#include <iostream>
#include <algorithm>


const std::vector<std::string> Parser::token_strings
{
	",",
	"NOP",
	"JUMP", "READ", "WRITE",
	"=", "JUMPT", "JUMPF",
	"+", "-", "*",
	"<", ">", "<=", ">=", "==",
	"(NUMBER)", "(VARIABLE)", "(EOL)"
};


Parser::Parser(std::string line) :
	m_line{ line }
{
	// Remove all whitespace from the line 
	m_line.erase(std::remove_if(m_line.begin(), m_line.end(), isspace), m_line.end());

	// Initialize stringstream -- we can't do this in initializer list
	m_ss.str(m_line);
}


Parser::Token Parser::next()
{
	std::string token;
	if (!std::getline(m_ss, token, ','))
	{
		return Token::EOL;
	}

	for (size_t i = 0; i < token_strings.size(); i++)
	{
		if (token_strings.at(i) == token)
		{
			m_tokenstr = token;
			return static_cast<Token>(i);
		}
	}

	if (!token.empty())
	{
		// No strings exist in this language, so variable is assumed
		// The actual validity of it is questionable
		if (isalpha(token[0]))
		{
			m_tokenstr = token;
			return Token::VARIABLE;
		}
		// Number
		else if (token[0] == '-' || isdigit(token[0]))
		{
			m_tokennum = std::stoi(token);
			return Token::NUMBER;
		}
	}

	return Token::NOP;
}


bool Parser::expect(Token t)
{
	return next() == t;
}


std::vector<Parser::Token> Parser::tokenize()
{
	std::vector<Token> tokens;

	Token t;
	do {
		t = next();
		tokens.push_back(t);
	} while (t != Token::EOL);

	return tokens;
}