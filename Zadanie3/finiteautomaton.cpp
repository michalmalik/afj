#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iterator>
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

	const int num_states = std::stoi(lines[0]);
	const int num_symbols = std::stoi(lines[1]);
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
			s.type = State::Type::INITIAL | State::Type::FINAL;
		}
		else if (type.length() == 1 && type == "I")
		{
			s.type = State::Type::INITIAL;
		}
		else if (type.length() == 1 && type == "F")
		{
			s.type = State::Type::FINAL;
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


FiniteAutomaton::Status FiniteAutomaton::write(const std::string &filename) const
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


bool FiniteAutomaton::accept(const std::string &s) const
{
	const auto it = std::find_if(m_states.begin(), m_states.end(), [](const std::pair<std::string, State> &p) -> bool {
		return p.second.isInitial();
	});

	if (it == m_states.end())
	{
		return false;
	}

	std::string state = (*it).first;

	size_t idx = 0;
	while (idx < s.size())
	{
		if (!m_state_table.count(state))
		{
			// State has no transitions
			return false;
		}

		const std::string sym(1, s[idx]);

		if (!m_state_table.at(state).count(sym))
		{
			// There's no transition from current state to new state using this symbol
			return false;
		}

		// It's possible to transition
		std::string new_state = Utils::join(m_state_table.at(state).at(sym), "");
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


std::set<std::string> FiniteAutomaton::closure(const std::set<std::string> &states, std::set<std::string> &done) const
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

		for (const std::string &x : m_state_table.at(state).at(""))
		{
			if (done.count(x))
			{
				continue;
			}

			done.insert(x);

			for (const std::string &s : closure({ x }, done))
			{
				empty.insert(s);
			}
		}
	}

	return empty;
}


std::set<std::string> FiniteAutomaton::transitions(const std::set<std::string> &from, const std::string &symbol) const
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


bool FiniteAutomaton::addState(const std::string &label, uint8_t type, bool mod)
{
	if (m_states.count(label))
	{
		if (mod)
		{
			m_states[label] = type;
			return true;
		}

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


#ifdef _TESTS
std::set<std::string> FiniteAutomaton::getStateTransitions(const std::string &st) const
{
	const auto state_transitions = m_state_table.find(st);
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
#endif // _TESTS


bool DFiniteAutomaton::operator==(const DFiniteAutomaton &rhs)
{
	// Alphabet has to be the same
	if (m_alphabet != rhs.getAlphabet())
	{
		return false;
	}

	// Same with number of possible states
	if (m_states.size() != rhs.getStates().size())
	{
		return false;
	}

	// States that have transitions should also be the same
	if (m_state_table.size() != rhs.getStateTable().size())
	{
		return false;
	}

	// We take all transition symbols and size of sets of states they transition to
	// add them to a vector, then sort them. In theory, they should be the same for
	// equivalent automatons.
	std::vector<std::string> left, right;
	for (const auto &p : m_state_table)
	{
		for (const auto &x : p.second)
		{
			std::string a = x.first + "" + std::to_string(x.second.size());
			left.push_back(a);
		}
	}

	for (const auto &p : rhs.getStateTable())
	{
		for (const auto &x : p.second)
		{
			std::string a = x.first + "" + std::to_string(x.second.size());
			right.push_back(a);
		}
	}

	std::sort(left.begin(), left.end());
	std::sort(right.begin(), right.end());

	if (Utils::join(left, "") != Utils::join(right, ""))
	{
		return false;
	}

	return true;
}