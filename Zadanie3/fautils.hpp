#pragma once

#include "regexp.hpp"
#include "finiteautomaton.hpp"


namespace FAUtils
{
	bool nfa_to_dfa(const NDFiniteAutomaton &nfa, DFiniteAutomaton &dfa);
	bool build_regexp_from_file(const std::string &filename);
}