#pragma once

#include <string>
#include <map>
#include <set>


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
		FILE_OPEN_FAILED,
	};

	FiniteAutomaton();

	/*
		[number of states]
		[number of alphabet symbols]
		[state_1 <I|F>]
		..
		[state_n <I|F>]
		[symbol_1]
		..
		[symbol_2]
		[transition_1]
		..
		[transition_n]
		<EOL>
	*/
	Status read(const std::string &filename);
	Status write(const std::string &filename);
	bool accept();

	/*
	Let's not even try :)

	bool operator==(const FiniteAutomaton &rhs);
	*/

	std::set<std::string> closure(std::set<std::string> states);
	std::set<std::string> newStates(const std::set<std::string> &from, const std::string &symbol);

	bool addState(const std::string &label, uint8_t type);
	void addSymbol(const std::string &symbol);
	bool addTransition(const std::string &from, const std::string &symbol, const std::string &to);

	const std::set<std::string> &getAlphabet() const { return m_alphabet; }
	const std::map<std::string, State> &getStates() const { return m_states; }
	const std::map<std::string, std::map<std::string, std::set<std::string>>> &getStateTable() const { return m_state_table; }

	void setAlphabet(const std::set<std::string> &alphabet) { m_alphabet = alphabet; }

	// Mainly used for tests
	std::set<std::string> getStateTransitions(const std::string &st);
};


class NDFiniteAutomaton : public FiniteAutomaton
{
public:
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
};