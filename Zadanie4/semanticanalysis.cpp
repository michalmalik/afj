#include <iostream>
#include <stack>

#include "semanticanalysis.hpp"


void SemanticAnalysis::buildConcat(const std::string &lhs, const std::string &rhs)
{
	const size_t left = reduce(lhs);
	const size_t right = reduce(rhs);

	// std::cout << "CONCAT: " << left << " + " << right << "\n";
	const std::string reduced = std::to_string(left) + "+" + std::to_string(right);

	if (!m_reduced_check.count(reduced))
	{
		m_expressions.push_back(m_expressions.at(left) + m_expressions.at(right));
		m_reduced_check.insert(reduced);
		m_reduced.push_back(reduced);
	}
}


void SemanticAnalysis::buildUnion(const std::string &lhs, const std::string &rhs)
{
	const size_t left = reduce(lhs);
	const size_t right = reduce(rhs);

	// std::cout << "UNION: " << left << " | " << right << "\n";
	const std::string reduced = std::to_string(left) + "|" + std::to_string(right);

	if (!m_reduced_check.count(reduced))
	{
		m_expressions.push_back(m_expressions.at(left) | m_expressions.at(right));
		m_reduced_check.insert(reduced);
		m_reduced.push_back(reduced);
	}
}


void SemanticAnalysis::buildIteration(const std::string &lhs)
{
	const size_t left = reduce(lhs);

	// std::cout << "ITER: " << left << "*\n";
	const std::string reduced = std::to_string(left) + "*";

	if (!m_reduced_check.count(reduced))
	{
		m_expressions.push_back(*m_expressions.at(left));
		m_reduced_check.insert(reduced);
		m_reduced.push_back(reduced);
	}
}


void SemanticAnalysis::buildSymbol(const std::string &s)
{
	if (s.length() != 1)
	{
		return;
	}

	if (!reduceExpression(s))
	{
		return;
	}

	if (!m_reduced_check.count(s))
	{
		if (s == " ")
		{
			m_expressions.push_back(RegExp(""));
		}
		else
		{
			m_expressions.push_back(RegExp(s));
		}

		m_reduced_check.insert(s);
		m_reduced.push_back(std::to_string(m_expressions.size() - 1));
	}
}


bool SemanticAnalysis::reduceExpression(const std::string &exp)
{
	if (m_reduce_table.count(exp))
	{
		return false;
	}

	if (exp.at(0) == '(' && isInPars(exp))
	{
		const size_t found = exp.find_last_of(")");
		if (found != std::string::npos)
		{
			const std::string inner = exp.substr(1, found - 1);
			const size_t index = m_reduce_table.at(inner);
			m_reduce_table.insert(std::make_pair(exp, index));
		}
	}
	else
	{
		m_reduce_table.insert(std::make_pair(exp, m_reduce_counter));
		++m_reduce_counter;
	}

	return true;
}


bool SemanticAnalysis::isInPars(const std::string &exp) const
{
	std::stack<std::pair<char, size_t>> s;

	size_t last_left = 0;
	size_t last_right = 0;
	for (size_t i = 0; i < exp.length(); i++)
	{
		char c = exp.at(i);
		switch (c)
		{
		case '(':
			{
				s.push(std::make_pair(c, i));
				break;
			}
		case ')':
			{
				auto p = s.top();
				s.pop();

				if (p.first != '(')
				{
					return false;
				}

				last_left = p.second;
				last_right = i;
				break;
			}
		}
	}

	return last_left == 0 && last_right == exp.length() - 1 && s.empty();
}