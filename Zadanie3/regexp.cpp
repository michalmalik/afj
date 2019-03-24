#include <iostream>
#include <set>
#include <map>

#include "regexp.hpp"


RegExp::RegExp()
{
	;
}


RegExp RegExp::operator+(RegExp &rhs)
{
	RegExp ret;
	NDFiniteAutomaton &new_nfa = ret.getAutomaton();

	std::set<std::string> left_final;

	// Transfer left symbols to new automaton
	for (const std::string & s : m_nfa.getAlphabet())
	{
		new_nfa.addSymbol(s);
	}

	// Transfer left states to new automaton
	// Remove final attribute from them
	for (const auto & p : m_nfa.getStates())
	{
		uint8_t type = p.second.type;

		if (p.second.isFinal())
		{
			type ^= State::FINAL;
			left_final.insert(p.first);
		}

		new_nfa.addState(p.first, type);
	}

	// Transfer left transitions to new automaton
	for (const auto & p : m_nfa.getStateTable())
	{
		const std::string & from = p.first;
		for (const auto & t : p.second)
		{
			const std::string & symbol = t.first;
			for (const std::string & to : t.second)
			{
				new_nfa.addTransition(from, symbol, to);
			}
		}
	}

	const NDFiniteAutomaton &right_nfa = rhs.getAutomaton();
	
	// Transfer right symbols to new automaton
	for (const std::string & s : right_nfa.getAlphabet())
	{
		new_nfa.addSymbol(s);
	}

	// old_q: new_q
	std::map<std::string, std::string> right_old_to_new;
	std::set<std::string> right_initial;

	// Transfer right states to new automaton
	// Remove initial attribute from them
	for (const auto & p : right_nfa.getStates())
	{
		uint8_t type = p.second.type;

		std::string new_q = p.first;
		if (m_nfa.getStates().count(p.first))
		{
			new_q += "n";
		}

		// p.first = old_q
		right_old_to_new.insert(std::make_pair(p.first, new_q));

		if (p.second.isInitial())
		{
			type ^= State::INITIAL;
			right_initial.insert(new_q);
		}

		new_nfa.addState(new_q, type);
	}

	// Transfer right transitions to new automaton
	for (const auto & p : right_nfa.getStateTable())
	{
		const std::string & from = right_old_to_new.at(p.first);
		for (const auto & t : p.second)
		{
			const std::string & symbol = t.first;
			for (const std::string & to : t.second)
			{
				new_nfa.addTransition(from, symbol, right_old_to_new.at(to));
			}
		}
	}

	// Concatenate left final states to right initial states
	// AKA create epsilon transition between them
	for (const std::string & lf : left_final)
	{
		for (const std::string & ri : right_initial)
		{
			new_nfa.addTransition(lf, "", ri);
		}
	}

	return ret;
}


RegExp RegExp::operator*(void)
{
	RegExp ret;
	return ret;
}


RegExp RegExp::operator|(RegExp &rhs)
{
	RegExp ret;
	return ret;
}


bool RegExp::createElementary(const std::string &symbol)
{
	std::set<std::string> s;
	for (char c : symbol)
	{
		s.insert(std::string{ c });
	}

	return createElementary(s);
}


bool RegExp::createElementary(const std::set<std::string> &symbol)
{
	if (symbol.size() > 1)
	{
		// Not allowed
		return false;
	}
	
	// Everything below should create an automaton in some form or another
	
	if (symbol.empty())
	{
		// Literally nothing
		m_nfa.addState("q0", State::INITIAL);
		return true;
	}

	// Only symbol of size 1 follows
	const std::string &s = *(symbol.begin());

	if (s.empty())
	{
		// Epsilon
		m_nfa.addState("q0", State::INITIAL | State::FINAL);
		return true;
	}

	m_nfa.addState("q0", State::INITIAL);
	m_nfa.addState("q1", State::FINAL);
	m_nfa.addSymbol(s);
	m_nfa.addTransition("q0", s, "q1");

	return true;
}