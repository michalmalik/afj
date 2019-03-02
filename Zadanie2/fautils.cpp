#include <algorithm>
#include <queue>

#include "fautils.hpp"
#include "utils.hpp"


bool FAUtils::nfa_to_dfa(FiniteAutomaton &nfa, FiniteAutomaton &dfa)
{
	std::set<std::string> initial_states, final_states;
	for (const auto &p : nfa.getStates())
	{
		if (p.second.isInitial())
			initial_states.insert(p.first);

		if (p.second.isFinal())
			final_states.insert(p.first);
	}

	// We don't care about epsilon because closure
	std::set<std::string> alphabet = nfa.getAlphabet();
	alphabet.erase("");

	dfa.setAlphabet(alphabet);

	std::set<std::string> closed_initial = nfa.closure(initial_states);

	uint8_t type = State::Type::INITIAL;
	if (std::any_of(closed_initial.begin(), closed_initial.end(), [nfa](const std::string &s) { return nfa.getStates().at(s).isFinal(); }))
	{
		type |= State::Type::FINAL;
	}

	dfa.addState(Utils::join(closed_initial, ""), type);

	std::queue<std::set<std::string>> q;
	q.push(closed_initial);

	while (!q.empty())
	{
		std::set<std::string> from = q.front();
		std::string from_str = Utils::join(from, "");
		q.pop();

		for (const std::string &symbol : alphabet)
		{
			std::set<std::string> to = nfa.closure(nfa.transitions(from, symbol));

			uint8_t type = State::Type::NONE;
			if (std::any_of(to.begin(), to.end(), [nfa](const std::string &s) { return nfa.getStates().at(s).isFinal(); }))
			{
				type |= State::Type::FINAL;
			}

			std::string to_str = Utils::join(to, "");

			if (!to.empty() && dfa.addState(to_str, type))
			{
				q.push(to);
			}

			dfa.addTransition(from_str, symbol, to_str);
		}
	}

	return true;
}