#pragma once

#include <string>
#include <map>
#include <set>


// This is a bit over-complicated for what it does, but eh :)
struct State
{
	enum Type : uint8_t
	{
		NONE = 0,
		INITIAL = 1 << 1,
		FINAL = 1 << 2
	};

	uint8_t type;

	State(uint8_t _type = NONE) :
		type{ _type }
	{
		;
	}

	bool operator==(const State &rhs)
	{
		return type == rhs.type;
	}

	bool operator!=(const State &rhs)
	{
		return type != rhs.type;
	}

	bool isInitial() const { return (type & INITIAL) == INITIAL; }
	bool isFinal() const { return (type & FINAL) == FINAL; }
};


class FiniteAutomaton
{
protected:
	std::set<std::string> m_alphabet;
	
	// q0: State, ..
	std::map<std::string, State> m_states;

	// q0: { a: { q1, q2 }, b: { q3 } } 
	std::map<std::string, std::map<std::string, std::set<std::string>>> m_state_table;

	FiniteAutomaton(const std::set<std::string> &alphabet);

public:
	enum Status
	{
		OK,
		FILE_OPEN_FAILED
	};

	FiniteAutomaton();

	/*
	Format of input and output file is as follows:

	[number of states]
	[number of alphabet symbols]
	[state_1 <I|F|IF>]
	..
	[state_n <I|F|IF>]
	[symbol_1]
	..
	[symbol_2]
	[transition_1]
	..
	[transition_n]
	<EOL>
	*/

	// TODO: read should be only in NDFiniteAutomaton
	// TODO: write should be only in DFiniteAutomaton
	Status read(const std::string &filename);
	Status write(const std::string &filename) const;

	// TODO: This should be only in DFiniteAutomaton
	bool accept(const std::string &s) const;

	std::set<std::string> closure(const std::set<std::string> &states, std::set<std::string> &done) const;
	std::set<std::string> transitions(const std::set<std::string> &from, const std::string &symbol) const;

	bool addState(const std::string &label, uint8_t type, bool mod = false);
	void addSymbol(const std::string &symbol);
	bool addTransition(const std::string &from, const std::string &symbol, const std::string &to);

	const std::set<std::string> &getAlphabet() const { return m_alphabet; }
	const std::map<std::string, State> &getStates() const { return m_states; }
	const std::map<std::string, std::map<std::string, std::set<std::string>>> &getStateTable() const { return m_state_table; }

	void setAlphabet(const std::set<std::string> &alphabet) { m_alphabet = alphabet; }

#ifdef _TESTS
	std::set<std::string> getStateTransitions(const std::string &st) const;
#endif // _TESTS
};


class NDFiniteAutomaton : public FiniteAutomaton
{
public:
	// This is a little hack because when we read automaton from a file
	// "epsilon" (empty) symbol is not actually defined, so we just assume it exists
	NDFiniteAutomaton() : FiniteAutomaton({ "" })
	{
		;
	}
};


class DFiniteAutomaton : public FiniteAutomaton
{
public:
	DFiniteAutomaton() : FiniteAutomaton({ })
	{
		;
	}

	// This is definitely not correct, but seems OK for our purpose here
	bool operator==(const DFiniteAutomaton &rhs);
};