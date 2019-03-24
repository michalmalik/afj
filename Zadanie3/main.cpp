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

	RegExp a("a"), b("b");
	RegExp c = a + b;

	const NDFiniteAutomaton &nfa = c.getAutomaton();
	DFiniteAutomaton dfa;
	FAUtils::nfa_to_dfa(nfa, dfa);

	std::cout << dfa.accept("ab") << "\n";

	for (const auto & p : nfa.getStates())
	{
		std::cout << p.first << ": " << (int)p.second.type << "\n";
	}

	for (const auto & p : nfa.getStateTable())
	{
		for (const auto & t : p.second)
		{
			for (const std::string & to : t.second)
			{
				std::cout << p.first << "->" << t.first << "->" << to << "\n";
			}
		}
	}

	return EXIT_SUCCESS;
}

#else

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

TEST_CASE("Elementary expressions")
{
	RegExp a, b("a"), c("ab");
	REQUIRE(!a.empty());
	REQUIRE(!b.empty());
	REQUIRE(c.empty());
}

TEST_CASE("Concatenation ab")
{
	RegExp a("a"), b("b");
	RegExp r = a + b;

	const NDFiniteAutomaton &nfa = r.getAutomaton();

	REQUIRE(nfa.getStates().size() == 4);
	REQUIRE(nfa.getStates().at("q0").isInitial());
	REQUIRE(nfa.getStates().at("q1").type == State::Type::NONE);
	REQUIRE(nfa.getStates().at("q2").type == State::Type::NONE);
	REQUIRE(nfa.getStates().at("q3").isFinal());

	auto q0 = nfa.getStateTransitions("q0");
	REQUIRE(q0.size() == 1);
	REQUIRE(q0.count("q0->a->q1"));
	auto q1 = nfa.getStateTransitions("q1");
	REQUIRE(q1.size() == 1);
	REQUIRE(q1.count("q1->->q2"));
	auto q2 = nfa.getStateTransitions("q2");
	REQUIRE(q2.size() == 1);
	REQUIRE(q2.count("q2->b->q3"));

	DFiniteAutomaton dfa;
	REQUIRE(FAUtils::nfa_to_dfa(nfa, dfa));
	REQUIRE(dfa.accept("ab"));
	REQUIRE(!dfa.accept("ba"));
	REQUIRE(!dfa.accept("a"));
	REQUIRE(!dfa.accept("b"));
}

TEST_CASE("Union a|b")
{
	RegExp a("a"), b("b");
	RegExp r = a | b;

	const NDFiniteAutomaton &nfa = r.getAutomaton();

	REQUIRE(nfa.getStates().size() == 5);
	REQUIRE(nfa.getStates().at("q4").isInitial());
	REQUIRE(nfa.getStates().at("q0").type == State::NONE);
	REQUIRE(nfa.getStates().at("q1").isFinal());
	REQUIRE(nfa.getStates().at("q2").type == State::NONE);
	REQUIRE(nfa.getStates().at("q3").isFinal());

	auto q4 = nfa.getStateTransitions("q4");
	REQUIRE(q4.size() == 2);
	REQUIRE(q4.count("q4->->q0"));
	REQUIRE(q4.count("q4->->q2"));
	auto q0 = nfa.getStateTransitions("q0");
	REQUIRE(q0.size() == 1);
	REQUIRE(q0.count("q0->a->q1"));
	auto q2 = nfa.getStateTransitions("q2");
	REQUIRE(q2.size() == 1);
	REQUIRE(q2.count("q2->b->q3"));

	DFiniteAutomaton dfa;
	REQUIRE(FAUtils::nfa_to_dfa(nfa, dfa));
	REQUIRE(dfa.accept("a"));
	REQUIRE(!dfa.accept("aa"));
	REQUIRE(dfa.accept("b"));
	REQUIRE(!dfa.accept("bb"));
}

TEST_CASE("Union a|E")
{
	RegExp a("a"), e("");
	RegExp r = a | e;

	const NDFiniteAutomaton &nfa = r.getAutomaton();
	DFiniteAutomaton dfa;
	REQUIRE(FAUtils::nfa_to_dfa(nfa, dfa));
	REQUIRE(dfa.accept("a"));
	REQUIRE(!dfa.accept("aa"));
}

TEST_CASE("Union + concatenation (ab|ba)")
{
	RegExp a("a"), b("b");
	RegExp x1 = a + b;
	RegExp x2 = b + a;
	RegExp r = x1 | x2;

	const NDFiniteAutomaton &nfa = r.getAutomaton();

	REQUIRE(nfa.getStates().size() == 9);
	REQUIRE(nfa.getStates().at("q8").isInitial());
	REQUIRE(nfa.getStates().at("q0").type == State::NONE);
	REQUIRE(nfa.getStates().at("q1").type == State::NONE);
	REQUIRE(nfa.getStates().at("q2").type == State::NONE);
	REQUIRE(nfa.getStates().at("q3").isFinal());
	REQUIRE(nfa.getStates().at("q4").type == State::NONE);
	REQUIRE(nfa.getStates().at("q5").type == State::NONE);
	REQUIRE(nfa.getStates().at("q6").type == State::NONE);
	REQUIRE(nfa.getStates().at("q7").isFinal());

	auto q8 = nfa.getStateTransitions("q8");
	REQUIRE(q8.size() == 2);
	REQUIRE(q8.count("q8->->q0"));
	REQUIRE(q8.count("q8->->q4"));
	auto q0 = nfa.getStateTransitions("q0");
	REQUIRE(q0.size() == 1);
	REQUIRE(q0.count("q0->a->q1"));
	auto q1 = nfa.getStateTransitions("q1");
	REQUIRE(q1.size() == 1);
	REQUIRE(q1.count("q1->->q2"));
	auto q2 = nfa.getStateTransitions("q2");
	REQUIRE(q2.size() == 1);
	REQUIRE(q2.count("q2->b->q3"));
	auto q3 = nfa.getStateTransitions("q3");
	REQUIRE(q3.size() == 0);
	auto q4 = nfa.getStateTransitions("q4");
	REQUIRE(q4.size() == 1);
	REQUIRE(q4.count("q4->b->q5"));
	auto q5 = nfa.getStateTransitions("q5");
	REQUIRE(q5.size() == 1);
	REQUIRE(q5.count("q5->->q6"));
	auto q6 = nfa.getStateTransitions("q6");
	REQUIRE(q6.size() == 1);
	REQUIRE(q6.count("q6->a->q7"));
	auto q7 = nfa.getStateTransitions("q7");
	REQUIRE(q7.size() == 0);

	DFiniteAutomaton dfa;
	REQUIRE(FAUtils::nfa_to_dfa(nfa, dfa));
	REQUIRE(dfa.accept("ab"));
	REQUIRE(!dfa.accept("aa"));
	REQUIRE(dfa.accept("ba"));
	REQUIRE(!dfa.accept("bb"));
}

TEST_CASE("Iteration a*")
{
	RegExp a("a");
	RegExp ai = *a;

	const NDFiniteAutomaton &nfa = ai.getAutomaton();

	REQUIRE(nfa.getStates().size() == 3);
	REQUIRE(nfa.getStates().at("q2").isInitial());
	REQUIRE(nfa.getStates().at("q0").type == State::NONE);
	REQUIRE(nfa.getStates().at("q1").isFinal());

	auto q0 = nfa.getStateTransitions("q0");
	REQUIRE(q0.size() == 1);
	REQUIRE(q0.count("q0->a->q1"));
	auto q1 = nfa.getStateTransitions("q1");
	REQUIRE(q1.size() == 1);
	REQUIRE(q1.count("q1->->q0"));
	auto q2 = nfa.getStateTransitions("q2");
	REQUIRE(q2.size() == 1);
	REQUIRE(q2.count("q2->->q0"));

	DFiniteAutomaton dfa;
	REQUIRE(FAUtils::nfa_to_dfa(nfa, dfa));
	REQUIRE(dfa.accept("a"));
	REQUIRE(dfa.accept("aa"));
	REQUIRE(dfa.accept("aaaa"));
	REQUIRE(dfa.accept("aaaaaaaa"));
	REQUIRE(dfa.accept("aaaaaaaaaaaaaaaa"));
}

TEST_CASE("Iteration + concat ab*")
{
	RegExp a("a"), b("b");
	RegExp bi = *b;
	RegExp r = a + bi;

	const NDFiniteAutomaton &nfa = r.getAutomaton();

	REQUIRE(nfa.getStates().size() == 5);
	REQUIRE(nfa.getStates().at("q0").isInitial());
	REQUIRE(nfa.getStates().at("q1").type == State::NONE);
	REQUIRE(nfa.getStates().at("q2").type == State::NONE);
	REQUIRE(nfa.getStates().at("q4").type == State::NONE);
	REQUIRE(nfa.getStates().at("q3").isFinal());

	auto q0 = nfa.getStateTransitions("q0");
	REQUIRE(q0.size() == 1);
	REQUIRE(q0.count("q0->a->q1"));
	auto q1 = nfa.getStateTransitions("q1");
	REQUIRE(q1.size() == 1);
	REQUIRE(q1.count("q1->->q4"));
	auto q2 = nfa.getStateTransitions("q2");
	REQUIRE(q2.size() == 1);
	REQUIRE(q2.count("q2->b->q3"));
	auto q3 = nfa.getStateTransitions("q3");
	REQUIRE(q3.size() == 1);
	REQUIRE(q3.count("q3->->q2"));
	auto q4 = nfa.getStateTransitions("q4");
	REQUIRE(q4.size() == 1);
	REQUIRE(q4.count("q4->->q2"));

	DFiniteAutomaton dfa;
	REQUIRE(FAUtils::nfa_to_dfa(nfa, dfa));
	REQUIRE(dfa.accept("ab"));
	REQUIRE(dfa.accept("abbbbbbbbbb"));
	REQUIRE(!dfa.accept("a"));
	REQUIRE(!dfa.accept("b"));
}

TEST_CASE("Iteration + concat a*b")
{
	RegExp a("a"), b("b");
	RegExp ai = *a;
	RegExp r = ai + b;

	const NDFiniteAutomaton &nfa = r.getAutomaton();

	nfa.write("nfa.txt");

	REQUIRE(nfa.getStates().size() == 5);
	REQUIRE(nfa.getStates().at("q2").isInitial());
	REQUIRE(nfa.getStates().at("q0").type == State::NONE);
	REQUIRE(nfa.getStates().at("q1").type == State::NONE);
	REQUIRE(nfa.getStates().at("q3").type == State::NONE);
	REQUIRE(nfa.getStates().at("q4").isFinal());

	auto q0 = nfa.getStateTransitions("q0");
	REQUIRE(q0.size() == 1);
	REQUIRE(q0.count("q0->a->q1"));
	auto q1 = nfa.getStateTransitions("q1");
	REQUIRE(q1.size() == 2);
	REQUIRE(q1.count("q1->->q0"));
	REQUIRE(q1.count("q1->->q3"));
	auto q2 = nfa.getStateTransitions("q2");
	REQUIRE(q2.size() == 1);
	REQUIRE(q2.count("q2->->q0"));
	auto q3 = nfa.getStateTransitions("q3");
	REQUIRE(q3.size() == 1);
	REQUIRE(q3.count("q3->b->q4"));
	auto q4 = nfa.getStateTransitions("q4");
	REQUIRE(q4.size() == 0);

	DFiniteAutomaton dfa;
	REQUIRE(FAUtils::nfa_to_dfa(nfa, dfa));

	REQUIRE(dfa.accept("ab"));
	REQUIRE(dfa.accept("aaaaaaaaab"));
	REQUIRE(!dfa.accept("a"));
	REQUIRE(!dfa.accept("aaaaaabbbbb"));
	REQUIRE(!dfa.accept("b"));
}

TEST_CASE("Iteration + concat (ab)*")
{
	RegExp a("a"), b("b");
	RegExp x1 = a + b;
	RegExp r = *x1;

	const NDFiniteAutomaton &nfa = r.getAutomaton();

	REQUIRE(nfa.getStates().size() == 5);
	REQUIRE(nfa.getStates().at("q4").isInitial());
	REQUIRE(nfa.getStates().at("q0").type == State::NONE);
	REQUIRE(nfa.getStates().at("q1").type == State::NONE);
	REQUIRE(nfa.getStates().at("q2").type == State::NONE);
	REQUIRE(nfa.getStates().at("q3").isFinal());

	auto q0 = nfa.getStateTransitions("q0");
	REQUIRE(q0.size() == 1);
	REQUIRE(q0.count("q0->a->q1"));
	auto q1 = nfa.getStateTransitions("q1");
	REQUIRE(q1.size() == 1);
	REQUIRE(q1.count("q1->->q2"));
	auto q2 = nfa.getStateTransitions("q2");
	REQUIRE(q2.size() == 1);
	REQUIRE(q2.count("q2->b->q3"));
	auto q3 = nfa.getStateTransitions("q3");
	REQUIRE(q3.size() == 1);
	REQUIRE(q3.count("q3->->q0"));
	auto q4 = nfa.getStateTransitions("q4");
	REQUIRE(q4.size() == 1);
	REQUIRE(q4.count("q4->->q0"));

	DFiniteAutomaton dfa;
	REQUIRE(FAUtils::nfa_to_dfa(nfa, dfa));
	REQUIRE(dfa.accept("ab"));
	REQUIRE(dfa.accept("abab"));
	REQUIRE(dfa.accept("ababab"));
	REQUIRE(dfa.accept("abababab"));
	REQUIRE(!dfa.accept("a"));
	REQUIRE(!dfa.accept("aba"));
	REQUIRE(!dfa.accept("ababa"));
}

TEST_CASE("Iteration + union (a|b)*")
{
	RegExp a("a"), b("b");
	RegExp x1 = a | b;
	RegExp r = *x1;

	const NDFiniteAutomaton &nfa = r.getAutomaton();

	DFiniteAutomaton dfa;
	REQUIRE(FAUtils::nfa_to_dfa(nfa, dfa));
	REQUIRE(dfa.accept("a"));
	REQUIRE(dfa.accept("b"));
	REQUIRE(dfa.accept("ab"));
	REQUIRE(dfa.accept("ba"));
	REQUIRE(dfa.accept("aa"));
	REQUIRE(dfa.accept("bb"));
}

TEST_CASE("Iteration + union a|b*")
{
	RegExp a("a"), b("b");
	RegExp bi = *b;
	RegExp r = a | bi;

	const NDFiniteAutomaton &nfa = r.getAutomaton();

	DFiniteAutomaton dfa;
	REQUIRE(FAUtils::nfa_to_dfa(nfa, dfa));
	REQUIRE(dfa.accept("b"));
	REQUIRE(dfa.accept("bbbbbbbbbbbb"));
	REQUIRE(dfa.accept("a"));
	REQUIRE(!dfa.accept("aa"));
	REQUIRE(!dfa.accept("ba"));
}

/*
TEST_CASE("Compound ab*(c|E)")
{
	// This regular expression cannot be made into a DFA
	RegExp a("a"), b("b");
	RegExp bi = *b;
	RegExp x1 = a + bi;
	RegExp c("c");
	RegExp e("");
	RegExp x2 = c | e;
	RegExp r = x1 + x2;

	const NDFiniteAutomaton &nfa = r.getAutomaton();

	DFiniteAutomaton dfa;
	REQUIRE(FAUtils::nfa_to_dfa(nfa, dfa));
	REQUIRE(dfa.accept("ab"));
	REQUIRE(dfa.accept("abc"));
	REQUIRE(dfa.accept("abbbbc"));
	REQUIRE(!dfa.accept("aba"));
}

TEST_CASE("Compound c*a(b|c)*")
{
	RegExp a("a"), b("b"), c("c");
	RegExp ci = *c;
	RegExp x1 = b | c;
	RegExp x1i = *x1;
	RegExp r = ci + a + x1i;

	const NDFiniteAutomaton &nfa = r.getAutomaton();

	DFiniteAutomaton dfa;
	REQUIRE(FAUtils::nfa_to_dfa(nfa, dfa));

	REQUIRE(dfa.accept("ccacb"));
	REQUIRE(!dfa.accept("ccccb"));
}
*/

#endif // _TESTS