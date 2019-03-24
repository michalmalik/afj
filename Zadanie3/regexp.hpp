#pragma once


#include <memory>

#include "finiteautomaton.hpp"


class RegExp
{
public:
	explicit RegExp();

	// Concatenation
	RegExp operator+(RegExp &rhs);

	// Iteration
	RegExp operator*(void);

	// Union
	RegExp operator|(RegExp &rhs);

	bool createElementary(const std::string &symbol);
	bool createElementary(const std::set<std::string> &symbol);

	NDFiniteAutomaton &getAutomaton() { return m_nfa; }

private:
	NDFiniteAutomaton m_nfa;
};