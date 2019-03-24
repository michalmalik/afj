#pragma once

#include "finiteautomaton.hpp"


namespace FAUtils
{
	bool nfa_to_dfa(const NDFiniteAutomaton &nfa, DFiniteAutomaton &dfa);
}