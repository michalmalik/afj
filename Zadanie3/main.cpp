#include <iostream>

#include "regexp.hpp"
#include "fautils.hpp"
#include "regexpbuilder.hpp"


#ifndef _TESTS

int main(int argc, char **argv)
{
	if (argc < 4)
	{
		std::cerr << "Usage: " << argv[0] << " <in_nfa> <final_nfa> <out_dfa>\n";
		return EXIT_FAILURE;
	}

	std::string in_expressions(argv[1]), final_nfa(argv[2]), out_dfa(argv[3]);

	RegExpBuilder reb;
	RegExpBuilder::Status status = reb.load(in_expressions);

	switch (status)
	{
	case RegExpBuilder::Status::FILE_OPEN_FAILED:
		{
			std::cerr << "Failed to open file \"" << in_expressions << "\"\n";
			return EXIT_FAILURE;
		}
	case RegExpBuilder::Status::OK:
		{
			break;
		}
	}

	RegExp &r = reb.getFinal();
	const NDFiniteAutomaton &nfa = r.getAutomaton();

	if (nfa.write(final_nfa) != FiniteAutomaton::Status::OK)
	{
		std::cerr << "Failed to write final NFA to \"" << final_nfa << "\"\n";
		return EXIT_FAILURE;
	}

	std::cout << "[+] NFA from final regexp written to \"" << final_nfa << "\"\n";

	DFiniteAutomaton dfa;
	if (FAUtils::nfa_to_dfa(nfa, dfa))
	{
		std::cout << "[+] Final NFA converted to DFA\n";
	}
	else
	{
		std::cerr << "Failed to convert final NFA to DFA\n";
		return EXIT_FAILURE;
	}

	if (dfa.write(out_dfa) != FiniteAutomaton::Status::OK)
	{
		std::cerr << "Failed to write DFA to \"" << out_dfa << "\"\n";
		return EXIT_FAILURE;
	}

	std::cout << "[+] DFA written to \"" << out_dfa << "\"\n";

	while (true)
	{
		std::cout << "Enter string to check (exit with Ctrl-C): ";

		std::string str;
		std::getline(std::cin, str);

		if (dfa.accept(str))
		{
			std::cout << "ACCEPT\n";
		}
		else
		{
			std::cout << "REJECT\n";
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
	REQUIRE(!FAUtils::is_dfa(nfa));

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
	REQUIRE(!FAUtils::is_dfa(nfa));

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
	REQUIRE(!FAUtils::is_dfa(nfa));

	DFiniteAutomaton dfa;
	REQUIRE(FAUtils::nfa_to_dfa(nfa, dfa));
	REQUIRE(dfa.accept(""));
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
	REQUIRE(!FAUtils::is_dfa(nfa));

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
	REQUIRE(!FAUtils::is_dfa(nfa));

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
	REQUIRE(dfa.accept(""));
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
	REQUIRE(!FAUtils::is_dfa(nfa));

	REQUIRE(nfa.getStates().size() == 5);
	REQUIRE(nfa.getStates().at("q0").isInitial());
	REQUIRE(nfa.getStates().at("q1").type == State::NONE);
	REQUIRE(nfa.getStates().at("q2").type == State::NONE);
	REQUIRE(nfa.getStates().at("q4").isFinal());
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
	REQUIRE(dfa.accept("a"));
	REQUIRE(!dfa.accept("b"));
}

TEST_CASE("Iteration + concat a*b")
{
	RegExp a("a"), b("b");
	RegExp ai = *a;
	RegExp r = ai + b;

	const NDFiniteAutomaton &nfa = r.getAutomaton();
	REQUIRE(!FAUtils::is_dfa(nfa));

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
	REQUIRE(q2.size() == 2);
	REQUIRE(q2.count("q2->->q0"));
	REQUIRE(q2.count("q2->->q3"));
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
	REQUIRE(dfa.accept("b"));
}

TEST_CASE("Iteration + concat (ab)*")
{
	RegExp a("a"), b("b");
	RegExp x1 = a + b;
	RegExp r = *x1;

	const NDFiniteAutomaton &nfa = r.getAutomaton();
	REQUIRE(!FAUtils::is_dfa(nfa));

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
	REQUIRE(dfa.accept(""));
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
	REQUIRE(!FAUtils::is_dfa(nfa));

	REQUIRE(nfa.getStates().size() == 6);
	REQUIRE(nfa.getStates().at("q5").isInitial());
	REQUIRE(nfa.getStates().at("q0").type == State::NONE);
	REQUIRE(nfa.getStates().at("q1").isFinal());
	REQUIRE(nfa.getStates().at("q2").type == State::NONE);
	REQUIRE(nfa.getStates().at("q3").isFinal());
	REQUIRE(nfa.getStates().at("q4").type == State::NONE);

	auto q5 = nfa.getStateTransitions("q5");
	REQUIRE(q5.size() == 1);
	REQUIRE(q5.count("q5->->q4"));
	auto q4 = nfa.getStateTransitions("q4");
	REQUIRE(q4.size() == 2);
	REQUIRE(q4.count("q4->->q0"));
	REQUIRE(q4.count("q4->->q2"));
	auto q3 = nfa.getStateTransitions("q3");
	REQUIRE(q3.size() == 1);
	REQUIRE(q3.count("q3->->q4"));
	auto q2 = nfa.getStateTransitions("q2");
	REQUIRE(q2.size() == 1);
	REQUIRE(q2.count("q2->b->q3"));
	auto q1 = nfa.getStateTransitions("q1");
	REQUIRE(q1.size() == 1);
	REQUIRE(q1.count("q1->->q4"));
	auto q0 = nfa.getStateTransitions("q0");
	REQUIRE(q0.size() == 1);
	REQUIRE(q0.count("q0->a->q1"));

	DFiniteAutomaton dfa;
	REQUIRE(FAUtils::nfa_to_dfa(nfa, dfa));
	REQUIRE(dfa.accept(""));
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
	REQUIRE(!FAUtils::is_dfa(nfa));

	DFiniteAutomaton dfa;
	REQUIRE(FAUtils::nfa_to_dfa(nfa, dfa));
	REQUIRE(dfa.accept("b"));
	REQUIRE(dfa.accept("bbbbbbbbbbbb"));
	REQUIRE(dfa.accept("a"));
	REQUIRE(!dfa.accept("aa"));
	REQUIRE(!dfa.accept("ba"));
}

TEST_CASE("Compound (E | ba)*c")
{
	RegExp a("a"), b("b"), c("c"), e("");
	RegExp ba = b + a;
	RegExp x1 = e | ba;
	RegExp x1i = *x1;
	RegExp r = x1i + c;

	const NDFiniteAutomaton &nfa = r.getAutomaton();
	REQUIRE(!FAUtils::is_dfa(nfa));

	DFiniteAutomaton dfa;
	REQUIRE(FAUtils::nfa_to_dfa(nfa, dfa));
	REQUIRE(dfa.accept("bac"));
	REQUIRE(!dfa.accept("ba"));
}

TEST_CASE("Compound ab*(c|E) -- wikipedia Regular Expressions")
{
	RegExp a("a"), b("b");
	RegExp bi = *b;
	RegExp x1 = a + bi;
	RegExp c("c");
	RegExp e("");
	RegExp x2 = c | e;
	RegExp r = x1 + x2;

	const NDFiniteAutomaton &nfa = r.getAutomaton();
	REQUIRE(!FAUtils::is_dfa(nfa));

	DFiniteAutomaton dfa;
	REQUIRE(FAUtils::nfa_to_dfa(nfa, dfa));
	REQUIRE(dfa.accept("ab"));
	REQUIRE(dfa.accept("ac"));
	REQUIRE(dfa.accept("abc"));
	REQUIRE(dfa.accept("abbbbc"));
	REQUIRE(!dfa.accept("aba"));
}

TEST_CASE("Compound (a|(b(ab*a)*b))* -- wikipedia Regular Expressions")
{
	RegExp a("a"), b("b");
	RegExp bi = *b;
	RegExp abi = a + bi;
	RegExp abia = abi + a;
	RegExp abiai = *abia;

	RegExp x1 = b + abiai + b;
	RegExp x = a | x1;
	RegExp r = *x;

	const NDFiniteAutomaton &nfa = r.getAutomaton();
	REQUIRE(!FAUtils::is_dfa(nfa));

	DFiniteAutomaton dfa;
	REQUIRE(FAUtils::nfa_to_dfa(nfa, dfa));
	REQUIRE(dfa.accept(""));
	REQUIRE(dfa.accept("a"));
	REQUIRE(dfa.accept("aa"));
	REQUIRE(dfa.accept("bb"));
	REQUIRE(dfa.accept("aaa"));
	REQUIRE(dfa.accept("abb"));
	REQUIRE(dfa.accept("bba"));
	REQUIRE(dfa.accept("aaaa"));
	REQUIRE(dfa.accept("aabb"));
	REQUIRE(dfa.accept("abba"));
	REQUIRE(dfa.accept("baab"));
	REQUIRE(dfa.accept("bbaa"));
	REQUIRE(dfa.accept("bbbb"));
}

TEST_CASE("Compound c*a(b|c)* -- zadanie 2, priklad 4")
{
	RegExp a("a"), b("b"), c("c");
	RegExp ci = *c;
	RegExp x1 = b | c;
	RegExp x1i = *x1;
	RegExp r = ci + a + x1i;

	const NDFiniteAutomaton &nfa = r.getAutomaton();
	REQUIRE(!FAUtils::is_dfa(nfa));

	DFiniteAutomaton dfa;
	REQUIRE(FAUtils::nfa_to_dfa(nfa, dfa));

	REQUIRE(dfa.accept("a"));
	REQUIRE(dfa.accept("abc"));
	REQUIRE(dfa.accept("ca"));
	REQUIRE(dfa.accept("cacb"));
	REQUIRE(dfa.accept("ccacb"));
	REQUIRE(!dfa.accept("ccccb"));
}

TEST_CASE("Compound ((ba)* | (ca)*)bb* -- zadanie 2, priklad 5")
{
	RegExp a("a"), b("b"), c("c");
	RegExp ba = b + a;
	RegExp ca = c + a;
	RegExp bi = *b;
	
	RegExp bai = *ba;
	RegExp cai = *ca;
	RegExp x1 = bai | cai;
	RegExp x = x1 + b;
	RegExp r = x + bi;

	const NDFiniteAutomaton &nfa = r.getAutomaton();
	REQUIRE(!FAUtils::is_dfa(nfa));

	DFiniteAutomaton dfa;
	REQUIRE(FAUtils::nfa_to_dfa(nfa, dfa));
	REQUIRE(dfa.accept("babab"));
	REQUIRE(!dfa.accept("bacab"));
}

TEST_CASE("Builder nothing.txt -- literal void")
{
	RegExpBuilder reb;
	REQUIRE(reb.load("tests/nothing.txt") == RegExpBuilder::Status::OK);
	REQUIRE(reb.getExpressions().size() == 1);

	const NDFiniteAutomaton &nfa = reb.getFinal().getAutomaton();
	REQUIRE(FAUtils::is_dfa(nfa));

	DFiniteAutomaton dfa;
	REQUIRE(FAUtils::nfa_to_dfa(nfa, dfa));
	REQUIRE(!dfa.accept(""));
	REQUIRE(!dfa.accept("a"));
	REQUIRE(!dfa.accept("b"));
}

TEST_CASE("Builder epsilon.txt -- only epsilon")
{
	RegExpBuilder reb;
	REQUIRE(reb.load("tests/epsilon.txt") == RegExpBuilder::Status::OK);
	REQUIRE(reb.getExpressions().size() == 1);

	const NDFiniteAutomaton &nfa = reb.getFinal().getAutomaton();
	REQUIRE(FAUtils::is_dfa(nfa));

	DFiniteAutomaton dfa;
	REQUIRE(FAUtils::nfa_to_dfa(nfa, dfa));
	REQUIRE(dfa.accept(""));
	REQUIRE(!dfa.accept("a"));
	REQUIRE(!dfa.accept("b"));
}

TEST_CASE("Builder 1.txt -- aa*")
{
	RegExpBuilder reb;
	REQUIRE(reb.load("tests/1.txt") == RegExpBuilder::Status::OK);
	REQUIRE(reb.getExpressions().size() == 3);

	const NDFiniteAutomaton &nfa = reb.getFinal().getAutomaton();
	REQUIRE(!FAUtils::is_dfa(nfa));

	DFiniteAutomaton dfa;

	REQUIRE(nfa.getStates().size() == 5);
	REQUIRE(nfa.getStates().at("q0").isInitial());
	REQUIRE(nfa.getStates().at("q1").type == State::NONE);
	REQUIRE(nfa.getStates().at("q2").type == State::NONE);
	REQUIRE(nfa.getStates().at("q4").isFinal());
	REQUIRE(nfa.getStates().at("q3").isFinal());

	auto q0 = nfa.getStateTransitions("q0");
	REQUIRE(q0.size() == 1);
	REQUIRE(q0.count("q0->a->q1"));
	auto q1 = nfa.getStateTransitions("q1");
	REQUIRE(q1.size() == 1);
	REQUIRE(q1.count("q1->->q4"));
	auto q2 = nfa.getStateTransitions("q2");
	REQUIRE(q2.size() == 1);
	REQUIRE(q2.count("q2->a->q3"));
	auto q3 = nfa.getStateTransitions("q3");
	REQUIRE(q3.size() == 1);
	REQUIRE(q3.count("q3->->q2"));
	auto q4 = nfa.getStateTransitions("q4");
	REQUIRE(q4.size() == 1);
	REQUIRE(q4.count("q4->->q2"));

	REQUIRE(FAUtils::nfa_to_dfa(nfa, dfa));
	REQUIRE(dfa.accept("aa"));
	REQUIRE(dfa.accept("aaaa"));
	REQUIRE(dfa.accept("a"));
}

TEST_CASE("Builder 2.txt -- E|ab")
{
	RegExpBuilder reb;
	REQUIRE(reb.load("tests/2.txt") == RegExpBuilder::Status::OK);
	REQUIRE(reb.getExpressions().size() == 5);

	const NDFiniteAutomaton &nfa = reb.getFinal().getAutomaton();
	REQUIRE(!FAUtils::is_dfa(nfa));

	REQUIRE(nfa.getStates().size() == 6);
	REQUIRE(nfa.getStates().at("q5").isInitial());
	REQUIRE(nfa.getStates().at("q1").type == State::NONE);
	REQUIRE(nfa.getStates().at("q2").type == State::NONE);
	REQUIRE(nfa.getStates().at("q3").type == State::NONE);
	REQUIRE(nfa.getStates().at("q4").isFinal());
	REQUIRE(nfa.getStates().at("q0").isFinal());

	auto q0 = nfa.getStateTransitions("q0");
	REQUIRE(q0.size() == 0);
	auto q1 = nfa.getStateTransitions("q1");
	REQUIRE(q1.size() == 1);
	REQUIRE(q1.count("q1->a->q2"));
	auto q2 = nfa.getStateTransitions("q2");
	REQUIRE(q2.size() == 1);
	REQUIRE(q2.count("q2->->q3"));
	auto q3 = nfa.getStateTransitions("q3");
	REQUIRE(q3.size() == 1);
	REQUIRE(q3.count("q3->b->q4"));
	auto q4 = nfa.getStateTransitions("q4");
	REQUIRE(q4.size() == 0);
	auto q5 = nfa.getStateTransitions("q5");
	REQUIRE(q5.size() == 2);
	REQUIRE(q5.count("q5->->q0"));
	REQUIRE(q5.count("q5->->q1"));

	DFiniteAutomaton dfa;
	REQUIRE(FAUtils::nfa_to_dfa(nfa, dfa));
	REQUIRE(dfa.accept(""));
	REQUIRE(dfa.accept("ab"));
	REQUIRE(!dfa.accept("aa"));
}

TEST_CASE("Builder 3.txt -- (a|b)*")
{
	RegExpBuilder reb;
	REQUIRE(reb.load("tests/3.txt") == RegExpBuilder::Status::OK);
	REQUIRE(reb.getExpressions().size() == 4);

	const NDFiniteAutomaton &nfa = reb.getFinal().getAutomaton();
	REQUIRE(!FAUtils::is_dfa(nfa));

	REQUIRE(nfa.getStates().size() == 6);
	REQUIRE(nfa.getStates().at("q5").isInitial());
	REQUIRE(nfa.getStates().at("q0").type == State::NONE);
	REQUIRE(nfa.getStates().at("q1").isFinal());
	REQUIRE(nfa.getStates().at("q2").type == State::NONE);
	REQUIRE(nfa.getStates().at("q3").isFinal());
	REQUIRE(nfa.getStates().at("q4").type == State::NONE);

	auto q5 = nfa.getStateTransitions("q5");
	REQUIRE(q5.size() == 1);
	REQUIRE(q5.count("q5->->q4"));
	auto q4 = nfa.getStateTransitions("q4");
	REQUIRE(q4.size() == 2);
	REQUIRE(q4.count("q4->->q0"));
	REQUIRE(q4.count("q4->->q2"));
	auto q3 = nfa.getStateTransitions("q3");
	REQUIRE(q3.size() == 1);
	REQUIRE(q3.count("q3->->q4"));
	auto q2 = nfa.getStateTransitions("q2");
	REQUIRE(q2.size() == 1);
	REQUIRE(q2.count("q2->b->q3"));
	auto q1 = nfa.getStateTransitions("q1");
	REQUIRE(q1.size() == 1);
	REQUIRE(q1.count("q1->->q4"));
	auto q0 = nfa.getStateTransitions("q0");
	REQUIRE(q0.size() == 1);
	REQUIRE(q0.count("q0->a->q1"));

	DFiniteAutomaton dfa;
	REQUIRE(FAUtils::nfa_to_dfa(nfa, dfa));
	REQUIRE(dfa.accept(""));
	REQUIRE(dfa.accept("a"));
	REQUIRE(dfa.accept("b"));
	REQUIRE(dfa.accept("ab"));
	REQUIRE(dfa.accept("ba"));
	REQUIRE(dfa.accept("aa"));
	REQUIRE(dfa.accept("bb"));
}

TEST_CASE("Builder 4.txt -- (E|ba)*c")
{
	RegExpBuilder reb;
	REQUIRE(reb.load("tests/4.txt") == RegExpBuilder::Status::OK);
	REQUIRE(reb.getExpressions().size() == 8);

	const NDFiniteAutomaton &nfa = reb.getFinal().getAutomaton();
	REQUIRE(!FAUtils::is_dfa(nfa));

	DFiniteAutomaton dfa;
	REQUIRE(FAUtils::nfa_to_dfa(nfa, dfa));
	REQUIRE(dfa.accept("bac"));
	REQUIRE(dfa.accept("babac"));
	REQUIRE(dfa.accept("c"));
	REQUIRE(!dfa.accept("ba"));
}

TEST_CASE("Builder 5.txt -- ((acb)*|E)a*b")
{
	RegExpBuilder reb;
	REQUIRE(reb.load("tests/5.txt") == RegExpBuilder::Status::OK);
	REQUIRE(reb.getExpressions().size() == 13);

	const NDFiniteAutomaton &nfa = reb.getFinal().getAutomaton();
	REQUIRE(!FAUtils::is_dfa(nfa));

	DFiniteAutomaton dfa;
	REQUIRE(FAUtils::nfa_to_dfa(nfa, dfa));
	REQUIRE(dfa.accept("acbaaab"));
	REQUIRE(dfa.accept("acbb"));
	REQUIRE(dfa.accept("acbacbab"));
	REQUIRE(dfa.accept("ab"));
	REQUIRE(dfa.accept("b"));
	REQUIRE(!dfa.accept("acbaaabbb"));
}

#endif // _TESTS