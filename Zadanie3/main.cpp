#include <iostream>

#include "regexp.hpp"
#include "fautils.hpp"

#ifndef _TESTS

int main(int argc, char **argv)
{
	/*
	if (argc < 3)
	{
		std::cerr << "Usage: " << argv[0] << " <in_nfa> <out_dfa>\n";
		return EXIT_FAILURE;
	}

	std::string in_nfa(argv[1]), out_dfa(argv[2]);
	*/

	RegExp a, b;

	std::cout << a.createElementary("a") << "\n";
	std::cout << b.createElementary("b") << "\n";

	RegExp c = a + b;

	DFiniteAutomaton dfa;
	FAUtils::nfa_to_dfa(c.getAutomaton(), dfa);

	std::cout << dfa.accept("ba") << "\n";


	/*
	for (const auto & p : nfa->getStates())
	{
		std::cout << p.first << ": " << (int)p.second.type << "\n";
	}

	for (const auto & p : nfa->getStateTable())
	{
		for (const auto & t : p.second)
		{
			for (const std::string & to : t.second)
			{
				std::cout << p.first << " -> " << t.first << " -> " << to << "\n";
			}
		}
	}
	*/

	return EXIT_SUCCESS;
}

#else

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

TEST_CASE("Elementary expressions")
{
	RegExp a, b, c, d;
	REQUIRE(a.createElementary(std::set<std::string>{}));
	REQUIRE(b.createElementary(std::set<std::string>{ "" }));
	REQUIRE(c.createElementary(std::set<std::string>{ "a" }));
	REQUIRE(!d.createElementary(std::set<std::string>{ "a", "b" }));
}

TEST_CASE("Concatenation 1")
{
	RegExp a, b;

	REQUIRE(a.createElementary("a"));
	REQUIRE(b.createElementary("b"));

	RegExp c = a + b;
	const NDFiniteAutomaton &nfa = c.getAutomaton();

	REQUIRE(nfa.getStates().size() == 4);

	DFiniteAutomaton dfa;
	REQUIRE(FAUtils::nfa_to_dfa(nfa, dfa));
	REQUIRE(dfa.accept("ab"));
	REQUIRE(!dfa.accept("ba"));
	REQUIRE(!dfa.accept("a"));
	REQUIRE(!dfa.accept("b"));
}

#endif // _TESTS