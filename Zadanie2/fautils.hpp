#pragma once

#include "finiteautomaton.hpp"

// The only point of this namespace is the fact that we expect
// more utilities to go here
namespace FAUtils
{
	bool nfa_to_dfa(FiniteAutomaton &nfa, FiniteAutomaton &dfa);
}