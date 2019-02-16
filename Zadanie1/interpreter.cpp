#include "interpreter.hpp"
#include "parser.hpp"

#include <fstream>
#include <iostream>
#include <algorithm>


Interpreter::Interpreter(std::string filename) :
	m_filename{ filename }
{
	;
}


Interpreter::Status Interpreter::execute()
{
	std::ifstream in(m_filename);
	if (!in.is_open())
	{
		return FILE_NOT_FOUND;
	}

	std::vector<std::string> lines;

	std::string line;
	while (std::getline(in, line))
	{
		if (!line.empty())
		{
			lines.push_back(line);
		}
	}

	in.close();

	if (lines.empty())
	{
		return NOTHING_TO_DO;
	}

	std::cout << "Lines: " << lines.size() << "\n";

	for (size_t i = 0; i < lines.size(); i++)
	{
		Parser p(lines.at(i));

		Parser::Token t;
		while ((t = p.next()))
		{
			if (t == Parser::Token::EOL)
			{
				break;
			}

			std::cout << Parser::token_strings[t] << "\n";
		}
	}

	return OK;
}