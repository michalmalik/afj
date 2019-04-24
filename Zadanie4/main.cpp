#include <iostream>
#include <fstream>

#include "lexer.hpp"
#include "parser.hpp"
#include "finiteautomaton.hpp"
#include "semanticanalysis.hpp"
#include "fautils.hpp"


extern SemanticAnalysis *sem;


std::string readRegexpFromFile(const std::string &filename)
{
	std::ifstream in_file(filename);
	if (!in_file.is_open())
	{
		return std::string{};
	}

	std::string regexp;
	std::getline(in_file, regexp);
	return regexp;
}


#ifndef _TESTS

int main(int argc, char **argv)
{
	if (argc < 4)
	{
		std::cerr << "Usage: " << argv[0] << " <in_regexp> <out_nfa> <out_dfa>\n";
		return EXIT_FAILURE;
	}

	const std::string in_regexp_file(argv[1]), out_nfa(argv[2]), out_dfa(argv[3]);
	const std::string in_regexp = readRegexpFromFile(in_regexp_file);

	if (in_regexp.empty())
	{
		std::cerr << "No regexp loaded from file\n";
		return EXIT_FAILURE;
	}

	yy_scan_string(in_regexp.c_str());

	std::cout << "[+] Parsing regexp: \"" << in_regexp << "\"\n";
	int status = yyparse();
	if (status != 0)
	{
		std::cerr << "Failed to parse regexp\n";
		return EXIT_FAILURE;
	}

	std::cout << "[ Expression table ]\n";
	for (const auto & p : sem->getExpressionTable())
	{
		std::cout << p.first << ": " << p.second << "\n";
	}

	std::cout << "[ Reduced table ]\n";
	size_t i = 0;
	for (const std::string & s : sem->getReducedExps())
	{
		std::cout << i << " = " << s << "\n";
		++i;
	}

	std::cout << "Expression table size: " << sem->getExpressionTable().size() << "\n";
	std::cout << "Expressions size: " << sem->getExpressions().size() << "\n";
	
	RegExp &r = sem->getFinal();
	const NDFiniteAutomaton &nfa = r.getAutomaton();

	if (nfa.write(out_nfa) != FiniteAutomaton::Status::OK)
	{
		std::cerr << "Failed to write final NFA to \"" << out_nfa << "\"\n";
		return EXIT_FAILURE;
	}

	std::cout << "[+] NFA from regexp written to \"" << out_nfa << "\"\n";

	DFiniteAutomaton dfa;
	if (FAUtils::nfa_to_dfa(nfa, dfa))
	{
		std::cout << "[+] NFA converted to DFA\n";
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

TEST_CASE("1.txt -- (aa*b)|(ab*b)")
{
	yy_scan_string("(aa*b)|(ab*b)");
	REQUIRE(yyparse() == 0);

	RegExp &r = sem->getFinal();
	const NDFiniteAutomaton &nfa = r.getAutomaton();
	DFiniteAutomaton dfa;

	REQUIRE(FAUtils::nfa_to_dfa(nfa, dfa));
	REQUIRE(dfa.accept("aaaab"));
	REQUIRE(!dfa.accept("aabab"));
}

TEST_CASE("2.txt -- (ba)*( |c)b*")
{
	yy_scan_string("(ba)*( |c)b*");
	REQUIRE(yyparse() == 0);

	RegExp &r = sem->getFinal();
	const NDFiniteAutomaton &nfa = r.getAutomaton();
	DFiniteAutomaton dfa;

	REQUIRE(FAUtils::nfa_to_dfa(nfa, dfa));
	REQUIRE(dfa.accept("babacbbb"));
	REQUIRE(!dfa.accept("babaccbb"));
}

TEST_CASE("3.txt -- (ab|ba|bb|aa)(a|b)*")
{
	yy_scan_string("(ab|ba|bb|aa)(a|b)*");
	REQUIRE(yyparse() == 0);

	RegExp &r = sem->getFinal();
	const NDFiniteAutomaton &nfa = r.getAutomaton();
	DFiniteAutomaton dfa;

	REQUIRE(FAUtils::nfa_to_dfa(nfa, dfa));
	REQUIRE(dfa.accept("ab"));
	REQUIRE(dfa.accept("abaaaa"));
	REQUIRE(!dfa.accept("a"));
}

TEST_CASE("4.txt -- (A|B|C)((a|b|c)*| )(A|B|C)")
{
	yy_scan_string("(A|B|C)((a|b|c)*| )(A|B|C)");
	REQUIRE(yyparse() == 0);

	RegExp &r = sem->getFinal();
	const NDFiniteAutomaton &nfa = r.getAutomaton();
	DFiniteAutomaton dfa;

	REQUIRE(FAUtils::nfa_to_dfa(nfa, dfa));
	REQUIRE(dfa.accept("AbacB"));
	REQUIRE(!dfa.accept("ABacB"));
}

TEST_CASE("5.txt -- ((I|i)(f|nt))|((E|e)(nd|lse))|((B|b)(egin))")
{
	yy_scan_string("((I|i)(f|nt))|((E|e)(nd|lse))|((B|b)(egin))");
	REQUIRE(yyparse() == 0);

	RegExp &r = sem->getFinal();
	const NDFiniteAutomaton &nfa = r.getAutomaton();
	DFiniteAutomaton dfa;

	REQUIRE(FAUtils::nfa_to_dfa(nfa, dfa));
	REQUIRE(dfa.accept("If"));
	REQUIRE(dfa.accept("if"));
	REQUIRE(dfa.accept("Int"));
	REQUIRE(dfa.accept("int"));
	REQUIRE(dfa.accept("End"));
	REQUIRE(dfa.accept("end"));
	REQUIRE(dfa.accept("Else"));
	REQUIRE(dfa.accept("else"));
	REQUIRE(dfa.accept("Begin"));
	REQUIRE(dfa.accept("begin"));
}

#endif // _TESTS