#include <fstream>
#include <iostream>
#include <sstream>

#include "regexpbuilder.hpp"


RegExpBuilder::RegExpBuilder()
{
	;
}


RegExpBuilder::Status RegExpBuilder::load(const std::string &filename)
{
	std::ifstream in_file(filename);
	if (!in_file.is_open())
	{
		return Status::FILE_OPEN_FAILED;
	}

	std::vector<std::string> lines;
	std::string line;
	while (std::getline(in_file, line))
	{
		lines.push_back(line);
	}

	for (const std::string &l : lines)
	{
		if (l.empty())
		{
			// Epsilon
			m_expressions.push_back(RegExp(""));
			continue;
		}

		if (l.length() == 1)
		{
			// Ascii symbol
			m_expressions.push_back(RegExp(l));
			continue;
		}

		std::stringstream ss(l);

		std::string op;
		std::getline(ss, op, ',');

		if (op == "I")
		{
			std::string i1s;
			std::getline(ss, i1s, ',');

			size_t i = std::stoi(i1s);
			m_expressions.push_back(*m_expressions.at(i - 1));
		}
		else if (op == "C")
		{
			std::string i1s;
			std::getline(ss, i1s, ',');

			std::string i2s;
			std::getline(ss, i2s, ',');

			size_t i = std::stoi(i1s), j = std::stoi(i2s);
			m_expressions.push_back(m_expressions.at(i - 1) + m_expressions.at(j - 1));
		}
		else if (op == "U")
		{
			std::string i1s;
			std::getline(ss, i1s, ',');

			std::string i2s;
			std::getline(ss, i2s, ',');

			size_t i = std::stoi(i1s), j = std::stoi(i2s);
			m_expressions.push_back(m_expressions.at(i - 1) | m_expressions.at(j - 1));
		}
		else
		{
			// Ignore
			continue;
		}
	}

	return Status::OK;
}