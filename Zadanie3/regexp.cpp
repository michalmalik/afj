#include <iostream>
#include <set>
#include <map>

#include "regexp.hpp"


RegExp::RegExp()
{
	build(std::set<std::string>{ });
}


RegExp::RegExp(const std::string &symbol)
{
	if (symbol.empty())
	{
		build(std::set<std::string>{ "" });
	}
	else
	{
		std::set<std::string> s;

		for (char c : symbol)
		{
			s.insert(std::string{ c });
		}

		build(s);
	}
}


RegExp RegExp::operator+(RegExp &rhs)
{
	RegExp ret;
	NDFiniteAutomaton &new_nfa = ret.getAutomaton();

	// Transfer left symbols to new automaton
	for (const std::string & s : m_nfa.getAlphabet())
	{
		new_nfa.addSymbol(s);
	}

	// Transfer left states to new automaton
	// Remove final attribute from them
	std::map<std::string, std::string> left_old_to_new;
	std::set<std::string> left_final;
	size_t left_counter = 0;

	auto &states = new_nfa.getStates();
	for (const auto & p : m_nfa.getStates())
	{
		uint8_t type = p.second.type;

		std::string new_q = "q" + std::to_string(left_counter++);
		left_old_to_new.insert(std::make_pair(p.first, new_q));

		if (p.second.isFinal())
		{
			type ^= State::FINAL;
			left_final.insert(new_q);
		}

		new_nfa.addState(new_q, type, true);
	}

	// Transfer left transitions to new automaton
	for (const auto & p : m_nfa.getStateTable())
	{
		const std::string & from = left_old_to_new.at(p.first);
		for (const auto & t : p.second)
		{
			const std::string & symbol = t.first;
			for (const std::string & to : t.second)
			{
				new_nfa.addTransition(from, symbol, left_old_to_new.at(to));
			}
		}
	}

	const NDFiniteAutomaton &right_nfa = rhs.getAutomaton();
	
	// Transfer right symbols to new automaton
	for (const std::string & s : right_nfa.getAlphabet())
	{
		new_nfa.addSymbol(s);
	}

	// Transfer right states to new automaton
	// Remove initial attribute from them
	// old_q: new_q
	std::map<std::string, std::string> right_old_to_new;
	std::set<std::string> right_initial;
	size_t right_counter = left_counter;

	for (const auto & p : right_nfa.getStates())
	{
		uint8_t type = p.second.type;

		std::string new_q = "q" + std::to_string(right_counter++);
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


RegExp RegExp::operator|(RegExp &rhs)
{
	RegExp ret;
	NDFiniteAutomaton &new_nfa = ret.getAutomaton();
	std::set<std::string> old_initial;

	// Copy left alphabet
	for (const std::string & s : m_nfa.getAlphabet())
	{
		new_nfa.addSymbol(s);
	}

	// Transfer left states
	// Take left initial
	std::map<std::string, std::string> left_old_to_new;
	
	size_t left_counter = 0;
	for (const auto & p : m_nfa.getStates())
	{
		uint8_t type = p.second.type;

		std::string new_q = "q" + std::to_string(left_counter++);
		left_old_to_new.insert(std::make_pair(p.first, new_q));

		if (p.second.isInitial())
		{
			type ^= State::INITIAL;
			old_initial.insert(new_q);
		}

		new_nfa.addState(new_q, type, true);
	}

	// Transfer left transitions to new automaton
	for (const auto & p : m_nfa.getStateTable())
	{
		const std::string & from = left_old_to_new.at(p.first);
		for (const auto & t : p.second)
		{
			const std::string & symbol = t.first;
			for (const std::string & to : t.second)
			{
				new_nfa.addTransition(from, symbol, left_old_to_new.at(to));
			}
		}
	}

	// Copy right alphabet
	const NDFiniteAutomaton &right_nfa = rhs.getAutomaton();
	
	for (const std::string & s : right_nfa.getAlphabet())
	{
		new_nfa.addSymbol(s);
	}

	// Transfer right states to new automaton
	// Take right initial -- rename every state
	std::map<std::string, std::string> right_old_to_new;
	size_t right_counter = left_counter;

	for (const auto & p : right_nfa.getStates())
	{
		uint8_t type = p.second.type;

		std::string new_q = "q" + std::to_string(right_counter++);
		right_old_to_new.insert(std::make_pair(p.first, new_q));

		if (p.second.isInitial())
		{
			type ^= State::INITIAL;
			old_initial.insert(new_q);
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

	// Create new initial state
	// Create epsilon transitions from this state to old initial states
	std::string new_initial = "q" + std::to_string(right_counter);
	new_nfa.addState(new_initial, State::INITIAL);
	
	for (const std::string &oi : old_initial)
	{
		new_nfa.addTransition(new_initial, "", oi);
	}

	return ret;
}


RegExp RegExp::operator*(void)
{
	RegExp ret;
	NDFiniteAutomaton &new_nfa = ret.getAutomaton();

	// Copy all symbols
	new_nfa.setAlphabet(m_nfa.getAlphabet());

	std::set<std::string> initials, finals;

	size_t state_counter = 0;
	for (const auto & p : m_nfa.getStates())
	{
		// TO ASK: What if final state is also an initial state? Do we create an epsilon transition to itself? Huh..
		uint8_t type = p.second.type;

		if (p.second.isInitial())
		{
			type ^= State::INITIAL;
			initials.insert(p.first);
		}
		
		if (p.second.isFinal())
		{
			finals.insert(p.first);
		}

		new_nfa.addState(p.first, type, true);
		++state_counter;
	}

	// Copy all transitions
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

	// Make epsilon transitions from final states to former initial states
	for (const std::string &f : finals)
	{
		for (const std::string &i : initials)
		{
			new_nfa.addTransition(f, "", i);
		}
	}

	// Create new initial + final state & create epsilon transitions from it to former initial states
	std::string new_initial = "q" + std::to_string(state_counter);
	new_nfa.addState(new_initial, State::INITIAL | State::FINAL);

	for (const std::string &i : initials)
	{
		new_nfa.addTransition(new_initial, "", i);
	}

	return ret;
}


bool RegExp::build(const std::set<std::string> &symbol)
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