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
	/*
	if (*m_lineptr == '\n' || *m_lineptr == '\r')
	{
		return Token::EOL;
	}

	while (*m_lineptr <= ' ')
	{
		if (*m_lineptr == 0)
		{
			return Token::EOL;
		}

		m_lineptr++;
	}

	char c;
	switch ((c = *m_lineptr++))
	{
	case ',': return Token::COMMA;
	case '+': return Token::ADD;
	case '-': return Token::SUB;
	case '*': return Token::MULTIPLY;
	default:
		{
			m_lineptr--;

			char tokenstr[64] = { 0 };
			char *x = tokenstr;

			while (*m_lineptr >= 0x30 && *m_lineptr <= 0x7e)
				*x++ = *m_lineptr++;

			m_tokenstr = tokenstr;

			for (size_t i = 0; i < token_strings.size(); i++)
			{
				if (token_strings.at(i) == m_tokenstr)
				{
					return (Token)i;
				}
			}
		}
	}
	*/

	std::string token;
	if (!std::getline(m_ss, token, ','))
	{
		return Token::EOL;
	}


	for (size_t i = 0; i < token_strings.size(); i++)
	{
		if (token_strings.at(i) == token)
		{
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