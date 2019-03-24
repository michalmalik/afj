#pragma once


#include <vector>

#include "regexp.hpp"


class RegExpBuilder
{
public:
	enum Status
	{
		OK = 0,
		FILE_OPEN_FAILED
	};

	explicit RegExpBuilder();

	Status load(const std::string &filename);

	RegExp &getFinal() { return m_expressions.at(m_expressions.size() - 1); }

private:
	std::vector<RegExp> m_expressions;
};