#include "parser.hpp"

#include <iostream>
#include <algorithm>


const std::vector<std::string> Parser::token_strings
{
	"NOP",
	"JUMP", "READ", "WRITE",
	"=", "JUMPT", "JUMPF",
	"+", "-", "*",
	"<", ">", "<=", ">=", "==",
	"(NUMBER)", "(VARIABLE)", "(STRING)", "(EOL)"
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
		m_tokenstr = "(NEWLINE)";
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
		if (isalpha(token[0]))
		{
			// No strings exist in this language, so variable is assumed
			// The actual validity of it is questionable and checked by interpreter
			m_tokenstr = token;
			return Token::VARIABLE;
		}
		
		else if (token[0] == '-' || isdigit(token[0]))
		{
			// Number
			m_tokennum = std::stoi(token);
			return Token::NUMBER;
		}
		else
		{
			// Everything else is a string.. not that we are going to need it
			m_tokenstr = token;
			return Token::STRING;
		}
	}
	else
	{
		m_tokenstr = "(NO_DATA)";
		return Token::EOL;
	}

	return Token::NOP;
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