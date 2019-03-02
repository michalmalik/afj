#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iterator>
#include <map>
#include <vector>

#include "finiteautomaton.hpp"
#include "utils.hpp"


FiniteAutomaton::FiniteAutomaton(const std::set<std::string> &alphabet) :
	m_alphabet{ alphabet }
{
	;
}


FiniteAutomaton::FiniteAutomaton()
{
	;
}


FiniteAutomaton::Status FiniteAutomaton::read(const std::string &filename)
{
	std::ifstream in_file(filename);
	if (!in_file.is_open())
	{
		return Status::FILE_OPEN_FAILED;
	}

	std::vector<std::string> lines;
	std::string line;
	while (std::getline(in_file, line))
	{
		if (!line.empty())
		{
			lines.push_back(line);
		}
	}

	in_file.close();

	int num_states = std::stoi(lines[0]);
	int num_symbols = std::stoi(lines[1]);
	int end = 2;

	// Load states
	for (int i = end; i < end + num_states; i++)
	{
		std::stringstream ss(lines[i]);

		std::string label;
		std::getline(ss, label, ' ');

		std::string type;
		std::getline(ss, type, ' ');

		State s;
		if (type.length() == 2 && (type == "IF" || type == "FI"))
		{
			// TODO, do this properly heh
			s.type = State::Type::INITIAL | State::Type::FINAL;
		}
		else if (type.length() == 1)
		{
			s.type = (type == "I" ? State::Type::INITIAL : State::Type::FINAL);
		}

		m_states.insert(std::make_pair(label, s));
	}

	end += num_states;

	// Load alphabet symbols
	for (int i = end; i < end + num_symbols; i++)
	{
		std::stringstream ss(lines[i]);
		std::string symbol;
		std::getline(ss, symbol);
		m_alphabet.insert(symbol);
	}

	end += num_symbols;

	// Load state transitions
	for (int i = end; i < lines.size(); i++)
	{
		std::stringstream ss(lines[i]);

		std::string from;
		std::getline(ss, from, ',');

		std::string symbol;
		std::getline(ss, symbol, ',');

		std::string to;
		std::getline(ss, to, ',');

		addTransition(from, symbol, to);
	}

	return Status::OK;
}


FiniteAutomaton::Status FiniteAutomaton::write(const std::string &filename)
{
	std::ofstream out_file(filename, std::ios::trunc);
	if (!out_file.is_open())
	{
		return Status::FILE_OPEN_FAILED;
	}

	out_file << m_states.size() << "\n";
	out_file << m_alphabet.size() << "\n";

	// Write states
	for (const auto &p : m_states)
	{
		out_file << p.first;
		
		if (p.second.isInitial() && p.second.isFinal())
			out_file << " IF";
		else if (p.second.isInitial())
			out_file << " I";
		else if (p.second.isFinal())
			out_file << " F";

		out_file << "\n";
	}

	// Write alphabet
	for (const std::string &symbol : m_alphabet)
	{
		out_file << symbol << "\n";
	}

	// Write state transitions
	std::vector<std::string> transition{ 3 };
	for (const auto &p : m_state_table)
	{
		transition[0] = p.first;
		for (const auto &t : p.second)
		{
			transition[1] = t.first;
			for (const std::string &to : t.second)
			{
				transition[2] = to;
				out_file << Utils::join(transition, ",") << "\n";
			}
		}
	}

	out_file.close();
	return Status::OK;
}


bool FiniteAutomaton::accept(const std::string &s)
{
	auto it = std::find_if(m_states.begin(), m_states.end(), [](const std::pair<std::string, State> &p) -> bool {
		return p.second.isInitial();
	});

	if (it == m_states.end())
	{
		return false;
	}

	std::string state = (*it).first;
	// std::cout << "Initial state: " << state << "\n";

	size_t idx = 0;
	while (idx < s.size())
	{
		if (!m_state_table.count(state))
		{
			// State has no transitions
			// std::cout << "No transition from " << state << "\n";
			return false;
		}

		std::string sym(1, s[idx]);

		if (!m_state_table.at(state).count(sym))
		{
			// There's no transition from current state to new state using this symbol
			// std::cout << "No transition from " << state << " through " << sym << "\n";
			return false;
		}

		// It's possible to transition
		std::string new_state = Utils::join(m_state_table.at(state).at(sym), "");
		// std::cout << state << "-> " << sym << " -> " << new_state << "\n";
		state = new_state;

		if (idx + 1 >= s.size() && !m_states.at(state).isFinal())
		{
			// We moved to a new state, but there's nothing else to process &
			// the new state is not final, so not accepting
			return false;
		}

		idx++;
	}

	return true;
}


std::set<std::string> FiniteAutomaton::closure(std::set<std::string> states)
{
	std::set<std::string> empty = states;

	for (const std::string &state : states)
	{
		if (!m_state_table.count(state))
		{
			continue;
		}

		if (!m_state_table.at(state).count(""))
		{
			continue;
		}

		for (const std::string &s : m_state_table.at(state).at(""))
		{
			std::set<std::string> x = closure({ s });
			for (const std::string &s : x)
			{
				empty.insert(s);
			}
		}
	}

	return empty;
}


std::set<std::string> FiniteAutomaton::newStates(const std::set<std::string> &from, const std::string &symbol)
{
	std::set<std::string> new_states;

	for (const std::string &state : from)
	{
		if (!m_state_table.count(state))
		{
			continue;
		}

		if (!m_state_table.at(state).count(symbol))
		{
			continue;
		}

		for (const std::string &s : m_state_table.at(state).at(symbol))
		{
			new_states.insert(s);
		}
	}

	return new_states;
}


bool FiniteAutomaton::addState(const std::string &label, uint8_t type)
{
	if (m_states.count(label))
	{
		return false;
	}

	m_states.insert(std::make_pair(label, State(type)));
	return true;
}


void FiniteAutomaton::addSymbol(const std::string &symbol)
{
	m_alphabet.insert(symbol);
}


bool FiniteAutomaton::addTransition(const std::string &from, const std::string &symbol, const std::string &to)
{
	if (!m_states.count(from) || !m_states.count(to) || !m_alphabet.count(symbol))
	{
		return false;
	}

	if (!m_state_table.count(from))
	{
		std::map<std::string, std::set<std::string>> transition{
			{ symbol, { to } }
		};

		m_state_table.insert(std::make_pair(from, transition));
	}
	else
	{
		if (!m_state_table.at(from).count(symbol))
		{
			m_state_table.at(from).insert(std::make_pair(symbol, std::set<std::string>{ to }));
		}
		else
		{
			m_state_table.at(from).at(symbol).insert(to);
		}
	}

	return true;
}


std::set<std::string> FiniteAutomaton::getStateTransitions(const std::string &st)
{
	auto state_transitions = m_state_table.find(st);
	if (state_transitions == m_state_table.end())
	{
		return std::set<std::string>{};
	}

	std::set<std::string> transitions;

	for (const auto &p : state_transitions->second)
	{
		for (const std::string &ns : p.second)
		{
			transitions.insert(st + "->" + p.first + "->" + ns);
		}
	}

	return transitions;
}