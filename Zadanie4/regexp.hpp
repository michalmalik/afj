#pragma once


#include "finiteautomaton.hpp"


class RegExp
{
public:
	RegExp();
	RegExp(const std::string &symbol);

	// Concatenation
	RegExp operator+(RegExp &rhs);

	// Union
	RegExp operator|(RegExp &rhs);

	// Iteration
	RegExp operator*(void);

	bool empty() const { return m_nfa.getStates().size() == 0; }
	NDFiniteAutomaton &getAutomaton() { return m_nfa; }

private:
	bool buildElementary(const std::set<std::string> &symbol);

	NDFiniteAutomaton m_nfa;
};