#include <iostream>
#include <algorithm>
#include <queue>

#include "utils.hpp"
#include "finiteautomaton.hpp"



bool convert_to_dfa(FiniteAutomaton &nfa, FiniteAutomaton &dfa)
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
			std::set<std::string> to = nfa.closure(nfa.newStates(from, symbol));

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


#ifndef _TESTS

int main(int argc, char **argv)
{
	if (argc < 3)
	{
		std::cerr << "Usage: " << argv[0] << " <in_nfa> <out_dfa>\n";
		return EXIT_FAILURE;
	}

	std::string in_nfa(argv[1]), out_dfa(argv[2]);

	NDFiniteAutomaton nfa;
	std::cout << "Load: " << nfa.read(in_nfa) << "\n";

	DFiniteAutomaton dfa;
	convert_to_dfa(nfa, dfa);
	dfa.write(out_dfa);

	return EXIT_SUCCESS;
}

#else
#define CATCH_CONFIG_MAIN
#include "catch.hpp"


TEST_CASE("Load 1_nka.txt", "[load]")
{
	NDFiniteAutomaton fa;

	REQUIRE(fa.read("tests/1_nka.txt") == FiniteAutomaton::Status::OK);
	REQUIRE(fa.getAlphabet().size() == 3);
	REQUIRE(fa.getStates().size() == 2);
	REQUIRE(fa.getStates().at("q0").type == (State::Type::INITIAL | State::Type::FINAL));
	REQUIRE(fa.getStates().at("q1").type == State::Type::NONE);
	REQUIRE(fa.getAlphabet().count("a"));
	REQUIRE(fa.getAlphabet().count("b"));
	REQUIRE(fa.getAlphabet().count(""));
	auto q0 = fa.getStateTransitions("q0");
	REQUIRE(q0.size() == 3);
	REQUIRE(q0.count("q0->a->q0"));
	REQUIRE(q0.count("q0->a->q1"));
	REQUIRE(q0.count("q0->b->q0"));
	auto q1 = fa.getStateTransitions("q1");
	REQUIRE(q1.size() == 2);
	REQUIRE(q1.count("q1->b->q0"));
	REQUIRE(q1.count("q1->b->q1"));
}

TEST_CASE("Load 2_nka.txt", "[load]")
{
	NDFiniteAutomaton fa;

	REQUIRE(fa.read("tests/2_nka.txt") == FiniteAutomaton::Status::OK);
	REQUIRE(fa.getAlphabet().size() == 3);
	REQUIRE(fa.getStates().size() == 3);
	REQUIRE(fa.getStates().at("q0").type == (State::Type::INITIAL | State::Type::FINAL));
	REQUIRE(fa.getStates().at("q1").type == State::Type::NONE);
	REQUIRE(fa.getStates().at("q2").isFinal());
	REQUIRE(fa.getAlphabet().count("a"));
	REQUIRE(fa.getAlphabet().count("b"));
	REQUIRE(fa.getAlphabet().count(""));
	auto q0 = fa.getStateTransitions("q0");
	REQUIRE(q0.size() == 2);
	REQUIRE(q0.count("q0->a->q1"));
	REQUIRE(q0.count("q0->->q2"));
	auto q1 = fa.getStateTransitions("q1");
	REQUIRE(q1.size() == 1);
	REQUIRE(q1.count("q1->b->q1"));
	auto q2 = fa.getStateTransitions("q2");
	REQUIRE(q2.size() == 2);
	REQUIRE(q2.count("q2->a->q1"));
	REQUIRE(q2.count("q2->a->q2"));
}


TEST_CASE("Load 3_nka.txt", "[load]")
{
	NDFiniteAutomaton fa;

	REQUIRE(fa.read("tests/3_nka.txt") == FiniteAutomaton::Status::OK);
	REQUIRE(fa.getAlphabet().size() == 3);
	REQUIRE(fa.getStates().size() == 4);
	REQUIRE(fa.getStates().at("q0").isInitial());
	REQUIRE(fa.getStates().at("q1").type == State::Type::NONE);
	REQUIRE(fa.getStates().at("q2").isFinal());
	REQUIRE(fa.getStates().at("qf").isFinal());
	REQUIRE(fa.getAlphabet().count("a"));
	REQUIRE(fa.getAlphabet().count("b"));
	REQUIRE(fa.getAlphabet().count(""));
	auto q0 = fa.getStateTransitions("q0");
	REQUIRE(q0.size() == 2);
	REQUIRE(q0.count("q0->a->q1"));
	REQUIRE(q0.count("q0->->qf"));
	auto q1 = fa.getStateTransitions("q1");
	REQUIRE(q1.size() == 3);
	REQUIRE(q1.count("q1->a->q1"));
	REQUIRE(q1.count("q1->a->q2"));
	REQUIRE(q1.count("q1->b->qf"));
	auto q2 = fa.getStateTransitions("q2");
	REQUIRE(q2.size() == 2);
	REQUIRE(q2.count("q2->a->q2"));
	REQUIRE(q2.count("q2->b->qf"));
}

TEST_CASE("Load 4_nka.txt", "[load]")
{
	NDFiniteAutomaton fa;

	REQUIRE(fa.read("tests/4_nka.txt") == FiniteAutomaton::Status::OK);
	REQUIRE(fa.getAlphabet().size() == 4);
	REQUIRE(fa.getStates().size() == 11);
	REQUIRE(fa.getStates().at("q0").isInitial());
	REQUIRE(fa.getStates().at("q1").type == State::Type::NONE);
	REQUIRE(fa.getStates().at("q2").type == State::Type::NONE);
	REQUIRE(fa.getStates().at("q3").type == State::Type::NONE);
	REQUIRE(fa.getStates().at("q4").type == State::Type::NONE);
	REQUIRE(fa.getStates().at("q5").isFinal());
	REQUIRE(fa.getStates().at("q6").type == State::Type::NONE);
	REQUIRE(fa.getStates().at("q7").type == State::Type::NONE);
	REQUIRE(fa.getStates().at("q8").type == State::Type::NONE);
	REQUIRE(fa.getStates().at("q9").isFinal());
	REQUIRE(fa.getStates().at("q10").isFinal());
	REQUIRE(fa.getAlphabet().count("a"));
	REQUIRE(fa.getAlphabet().count("b"));
	REQUIRE(fa.getAlphabet().count("c"));
	REQUIRE(fa.getAlphabet().count(""));
	auto q0 = fa.getStateTransitions("q0");
	REQUIRE(q0.size() == 2);
	REQUIRE(q0.count("q0->->q1"));
	REQUIRE(q0.count("q0->->q3"));
	auto q1 = fa.getStateTransitions("q1");
	REQUIRE(q1.size() == 1);
	REQUIRE(q1.count("q1->c->q2"));
	auto q2 = fa.getStateTransitions("q2");
	REQUIRE(q2.size() == 2);
	REQUIRE(q2.count("q2->->q1"));
	REQUIRE(q2.count("q2->->q3"));
	auto q3 = fa.getStateTransitions("q3");
	REQUIRE(q3.size() == 1);
	REQUIRE(q3.count("q3->a->q4"));
	auto q4 = fa.getStateTransitions("q4");
	REQUIRE(q4.size() == 1);
	REQUIRE(q4.count("q4->->q5"));
	auto q5 = fa.getStateTransitions("q5");
	REQUIRE(q5.size() == 1);
	REQUIRE(q5.count("q5->->q6"));
	auto q6 = fa.getStateTransitions("q6");
	REQUIRE(q6.size() == 2);
	REQUIRE(q6.count("q6->->q7"));
	REQUIRE(q6.count("q6->->q8"));
	auto q7 = fa.getStateTransitions("q7");
	REQUIRE(q7.size() == 1);
	REQUIRE(q7.count("q7->b->q9"));
	auto q8 = fa.getStateTransitions("q8");
	REQUIRE(q8.size() == 1);
	REQUIRE(q8.count("q8->c->q10"));
	auto q9 = fa.getStateTransitions("q9");
	REQUIRE(q8.size() == 1);
	REQUIRE(q9.count("q9->->q6"));
	auto q10 = fa.getStateTransitions("q10");
	REQUIRE(q10.size() == 1);
	REQUIRE(q10.count("q10->->q6"));
}

TEST_CASE("Load 5_nka.txt", "[load]")
{
	NDFiniteAutomaton fa;

	REQUIRE(fa.read("tests/5_nka.txt") == FiniteAutomaton::Status::OK);
	REQUIRE(fa.getAlphabet().size() == 4);
	REQUIRE(fa.getStates().size() == 16);
	REQUIRE(fa.getStates().at("q1").isInitial());
	REQUIRE(fa.getStates().at("q2").type == State::Type::NONE);
	REQUIRE(fa.getStates().at("q3").type == State::Type::NONE);
	REQUIRE(fa.getStates().at("q4").type == State::Type::NONE);
	REQUIRE(fa.getStates().at("q5").type == State::Type::NONE);
	REQUIRE(fa.getStates().at("q6").type == State::Type::NONE);
	REQUIRE(fa.getStates().at("q7").type == State::Type::NONE);
	REQUIRE(fa.getStates().at("q8").type == State::Type::NONE);
	REQUIRE(fa.getStates().at("q9").type == State::Type::NONE);
	REQUIRE(fa.getStates().at("q10").type == State::Type::NONE);
	REQUIRE(fa.getStates().at("q11").type == State::Type::NONE);
	REQUIRE(fa.getStates().at("q12").type == State::Type::NONE);
	REQUIRE(fa.getStates().at("q13").type == State::Type::NONE);
	REQUIRE(fa.getStates().at("q14").isFinal());
	REQUIRE(fa.getStates().at("q15").type == State::Type::NONE);
	REQUIRE(fa.getStates().at("q16").isFinal());
	REQUIRE(fa.getAlphabet().count("a"));
	REQUIRE(fa.getAlphabet().count("b"));
	REQUIRE(fa.getAlphabet().count("c"));
	REQUIRE(fa.getAlphabet().count(""));
	auto q1 = fa.getStateTransitions("q1");
	REQUIRE(q1.size() == 2);
	REQUIRE(q1.count("q1->->q2"));
	REQUIRE(q1.count("q1->->q7"));
	auto q2 = fa.getStateTransitions("q2");
	REQUIRE(q2.size() == 2);
	REQUIRE(q2.count("q2->->q3"));
	REQUIRE(q2.count("q2->->q12"));
	auto q3 = fa.getStateTransitions("q3");
	REQUIRE(q3.size() == 1);
	REQUIRE(q3.count("q3->b->q4"));
	auto q4 = fa.getStateTransitions("q4");
	REQUIRE(q4.size() == 1);
	REQUIRE(q4.count("q4->->q5"));
	auto q5 = fa.getStateTransitions("q5");
	REQUIRE(q5.size() == 1);
	REQUIRE(q5.count("q5->a->q6"));
	auto q6 = fa.getStateTransitions("q6");
	REQUIRE(q6.size() == 2);
	REQUIRE(q6.count("q6->->q3"));
	REQUIRE(q6.count("q6->->q12"));
	auto q7 = fa.getStateTransitions("q7");
	REQUIRE(q7.size() == 2);
	REQUIRE(q7.count("q7->->q8"));
	REQUIRE(q7.count("q7->->q12"));
	auto q8 = fa.getStateTransitions("q8");
	REQUIRE(q8.size() == 1);
	REQUIRE(q8.count("q8->c->q9"));
	auto q9 = fa.getStateTransitions("q9");
	REQUIRE(q8.size() == 1);
	REQUIRE(q9.count("q9->->q10"));
	auto q10 = fa.getStateTransitions("q10");
	REQUIRE(q10.size() == 1);
	REQUIRE(q10.count("q10->a->q11"));
	auto q11 = fa.getStateTransitions("q11");
	REQUIRE(q11.size() == 2);
	REQUIRE(q11.count("q11->->q12"));
	REQUIRE(q11.count("q11->->q8"));
	auto q12 = fa.getStateTransitions("q12");
	REQUIRE(q12.size() == 1);
	REQUIRE(q12.count("q12->b->q13"));
	auto q13 = fa.getStateTransitions("q13");
	REQUIRE(q13.size() == 1);
	REQUIRE(q13.count("q13->->q14"));
	auto q14 = fa.getStateTransitions("q14");
	REQUIRE(q14.size() == 1);
	REQUIRE(q14.count("q14->->q15"));
	auto q15 = fa.getStateTransitions("q15");
	REQUIRE(q15.size() == 1);
	REQUIRE(q15.count("q15->b->q16"));
	auto q16 = fa.getStateTransitions("q16");
	REQUIRE(q16.size() == 1);
	REQUIRE(q16.count("q16->->q15"));
}

TEST_CASE("Load closure_test_nka.txt", "[load]")
{
	NDFiniteAutomaton fa;

	REQUIRE(fa.read("tests/closure_test_nka.txt") == FiniteAutomaton::Status::OK);
	REQUIRE(fa.getAlphabet().size() == 3);
	REQUIRE(fa.getStates().size() == 5);
	REQUIRE(fa.getStates().at("q0").isInitial());
	REQUIRE(fa.getStates().at("q1").isFinal());
	REQUIRE(fa.getStates().at("q2").type == State::Type::NONE);
	REQUIRE(fa.getStates().at("q3").type == State::Type::NONE);
	REQUIRE(fa.getStates().at("q4").isFinal());
	REQUIRE(fa.getAlphabet().count("a"));
	REQUIRE(fa.getAlphabet().count("b"));
	REQUIRE(fa.getAlphabet().count(""));
	auto q0 = fa.getStateTransitions("q0");
	REQUIRE(q0.size() == 2);
	REQUIRE(q0.count("q0->->q1"));
	REQUIRE(q0.count("q0->b->q3"));
	auto q1 = fa.getStateTransitions("q1");
	REQUIRE(q1.size() == 2);
	REQUIRE(q1.count("q1->a->q1"));
	REQUIRE(q1.count("q1->a->q2"));
	auto q2 = fa.getStateTransitions("q2");
	REQUIRE(q2.size() == 3);
	REQUIRE(q2.count("q2->b->q2"));
	REQUIRE(q2.count("q2->b->q4"));
	REQUIRE(q2.count("q2->->q0"));
	auto q3 = fa.getStateTransitions("q3");
	REQUIRE(q3.size() == 2);
	REQUIRE(q3.count("q3->a->q2"));
	REQUIRE(q3.count("q3->a->q1"));
	auto q4 = fa.getStateTransitions("q4");
	REQUIRE(q4.size() == 1);
	REQUIRE(q4.count("q4->a->q3"));
}

TEST_CASE("Closure closure_test_nka.txt", "[closure]")
{
	NDFiniteAutomaton fa;

	REQUIRE(fa.read("tests/closure_test_nka.txt") == FiniteAutomaton::Status::OK);
	REQUIRE(fa.closure({ "q0" }) == std::set<std::string>{ "q0", "q1" });
	REQUIRE(fa.closure({ "q1", "q2" }) == std::set<std::string>{ "q0", "q1", "q2" });
	REQUIRE(fa.closure({ "q3" }) == std::set<std::string>{ "q3" });
	REQUIRE(fa.closure({ "q2", "q3", "q4" }) == std::set<std::string>{ "q0", "q1", "q2", "q3", "q4" });
	REQUIRE(fa.closure({ }) == std::set<std::string>{ });
	REQUIRE(fa.closure({ "q1", "q2", "q3" }) == std::set<std::string>{ "q0", "q1", "q2", "q3" });
}

TEST_CASE("New states closure_test_nka.txt", "[closure]")
{
	NDFiniteAutomaton fa;

	REQUIRE(fa.read("tests/closure_test_nka.txt") == FiniteAutomaton::Status::OK);
	REQUIRE(fa.newStates({ "q0", "q1" }, "a") == std::set<std::string>{ "q1", "q2" });
	REQUIRE(fa.newStates({ "q0", "q1" }, "b") == std::set<std::string>{ "q3" });
	REQUIRE(fa.newStates({ "q0", "q1", "q2" }, "a") == std::set<std::string>{ "q1", "q2" });
	REQUIRE(fa.newStates({ "q0", "q1", "q2" }, "b") == std::set<std::string>{ "q2", "q3", "q4" });
	REQUIRE(fa.newStates({ "q3" }, "a") == std::set<std::string>{ "q1", "q2" });
	REQUIRE(fa.newStates({ "q3" }, "b") == std::set<std::string>{ });
	REQUIRE(fa.newStates({ "q0", "q1", "q2", "q3", "q4" }, "a") == std::set<std::string>{ "q1", "q2", "q3" });
	REQUIRE(fa.newStates({ "q0", "q1", "q2", "q3", "q4" }, "b") == std::set<std::string>{ "q2", "q3", "q4" });
	REQUIRE(fa.newStates({ "q0", "q1", "q2", "q3" }, "a") == std::set<std::string>{ "q1", "q2" });
	REQUIRE(fa.newStates({ "q0", "q1", "q2", "q3" }, "b") == std::set<std::string>{ "q2", "q3", "q4" });
}

TEST_CASE("Accept 1_nka.txt", "[accept]")
{
	NDFiniteAutomaton nka;
	DFiniteAutomaton dka, dka_test;

	REQUIRE(nka.read("tests/1_nka.txt") == FiniteAutomaton::Status::OK);
	REQUIRE(dka.read("tests/1_dka.txt") == FiniteAutomaton::Status::OK);

	convert_to_dfa(nka, dka_test);

	REQUIRE(dka.accept("abab"));
	REQUIRE(dka_test.accept("abab"));

	REQUIRE(dka.accept("baba"));
	REQUIRE(dka_test.accept("baba"));

	REQUIRE(dka.accept("bbbb"));
	REQUIRE(dka_test.accept("bbbb"));

	REQUIRE(!dka.accept("c"));
	REQUIRE(!dka_test.accept("c"));
}

TEST_CASE("Accept 2_nka.txt", "[accept]")
{
	NDFiniteAutomaton nka;
	DFiniteAutomaton dka, dka_test;

	REQUIRE(nka.read("tests/2_nka.txt") == FiniteAutomaton::Status::OK);
	REQUIRE(dka.read("tests/2_dka.txt") == FiniteAutomaton::Status::OK);

	convert_to_dfa(nka, dka_test);

	REQUIRE(dka.accept("aaaaa"));
	REQUIRE(dka_test.accept("aaaaa"));

	REQUIRE(!dka.accept("aabaa"));
	REQUIRE(!dka_test.accept("aabaa"));

	REQUIRE(!dka.accept("abb"));
	REQUIRE(!dka_test.accept("abb"));

	REQUIRE(!dka.accept("baa"));
	REQUIRE(!dka_test.accept("baa"));

	REQUIRE(!dka.accept("c"));
	REQUIRE(!dka_test.accept("c"));
}

TEST_CASE("Accept 3_nka.txt", "[accept]")
{
	NDFiniteAutomaton nka;
	DFiniteAutomaton dka, dka_test;

	REQUIRE(nka.read("tests/3_nka.txt") == FiniteAutomaton::Status::OK);
	REQUIRE(dka.read("tests/3_dka.txt") == FiniteAutomaton::Status::OK);

	convert_to_dfa(nka, dka_test);

	REQUIRE(dka.accept("aaaab"));
	REQUIRE(dka_test.accept("aaaab"));

	REQUIRE(dka.accept("ab"));
	REQUIRE(dka_test.accept("ab"));

	REQUIRE(dka.accept("aab"));
	REQUIRE(dka_test.accept("aab"));

	REQUIRE(!dka.accept("aabba"));
	REQUIRE(!dka_test.accept("aabba"));

	REQUIRE(!dka.accept("abb"));
	REQUIRE(!dka_test.accept("abb"));

	REQUIRE(!dka.accept("b"));
	REQUIRE(!dka_test.accept("b"));
}

TEST_CASE("Accept 4_nka.txt", "[accept]")
{
	NDFiniteAutomaton nka;
	DFiniteAutomaton dka, dka_test;

	REQUIRE(nka.read("tests/4_nka.txt") == FiniteAutomaton::Status::OK);
	REQUIRE(dka.read("tests/4_dka.txt") == FiniteAutomaton::Status::OK);

	convert_to_dfa(nka, dka_test);

	REQUIRE(dka.accept("ccacb"));
	REQUIRE(dka_test.accept("ccacb"));

	REQUIRE(dka.accept("ab"));
	REQUIRE(dka_test.accept("ab"));

	REQUIRE(dka.accept("abc"));
	REQUIRE(dka_test.accept("abc"));

	REQUIRE(dka.accept("abcccccccc"));
	REQUIRE(dka_test.accept("abcccccccc"));

	REQUIRE(!dka.accept("ccccb"));
	REQUIRE(!dka_test.accept("ccccb"));

	REQUIRE(!dka.accept("cb"));
	REQUIRE(!dka_test.accept("cb"));
}

TEST_CASE("Accept 5_nka.txt", "[accept]")
{
	NDFiniteAutomaton nka;
	DFiniteAutomaton dka, dka_test;

	REQUIRE(nka.read("tests/5_nka.txt") == FiniteAutomaton::Status::OK);
	REQUIRE(dka.read("tests/5_dka.txt") == FiniteAutomaton::Status::OK);

	convert_to_dfa(nka, dka_test);

	REQUIRE(dka.accept("babab"));
	REQUIRE(dka_test.accept("babab"));

	REQUIRE(!dka.accept("bacab"));
	REQUIRE(!dka_test.accept("bacab"));
}

#endif