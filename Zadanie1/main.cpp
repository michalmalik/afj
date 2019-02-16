#include "interpreter.hpp"

#include <iostream>


#ifdef _TESTS
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <fstream>
#include "parser.hpp"


std::vector<std::vector<Parser::Token>> tokenize_file(const std::string &filename)
{
	std::vector<std::vector<Parser::Token>> file_tokens;

	std::ifstream in(filename);
	if (!in.is_open())
	{
		return file_tokens;
	}

	std::vector<std::string> lines;

	std::string line;
	while (std::getline(in, line))
	{
		if (!line.empty())
		{
			lines.push_back(line);
		}
	}

	in.close();

	if (lines.empty())
	{
		return file_tokens;
	}

	for (const std::string &l : lines)
	{
		Parser p(l);
		file_tokens.push_back(p.tokenize());
	}

	return file_tokens;
}

TEST_CASE("Parser tests", "[parser]")
{
	std::vector<std::vector<Parser::Token>> t1{
		{ Parser::Token::READ, Parser::Token::VARIABLE, Parser::Token::EOL },
		{ Parser::Token::ASSIGN, Parser::Token::VARIABLE, Parser::Token::NUMBER, Parser::Token::EOL },
		{ Parser::Token::EQ, Parser::Token::VARIABLE, Parser::Token::NUMBER, Parser::Token::VARIABLE, Parser::Token::EOL },
		{ Parser::Token::JUMPT, Parser::Token::VARIABLE, Parser::Token::NUMBER, Parser::Token::EOL },
		{ Parser::Token::MULTIPLY, Parser::Token::VARIABLE, Parser::Token::VARIABLE, Parser::Token::VARIABLE, Parser::Token::EOL },
		{ Parser::Token::SUB, Parser::Token::VARIABLE, Parser::Token::NUMBER, Parser::Token::VARIABLE, Parser::Token::EOL },
		{ Parser::Token::JUMP, Parser::Token::NUMBER, Parser::Token::EOL },
		{ Parser::Token::WRITE, Parser::Token::VARIABLE, Parser::Token::EOL }
	};

	std::vector<std::vector<Parser::Token>> t3{
		{ Parser::Token::READ, Parser::Token::VARIABLE, Parser::Token::EOL },
		{ Parser::Token::LT, Parser::Token::VARIABLE, Parser::Token::NUMBER, Parser::Token::VARIABLE, Parser::Token::EOL },
		{ Parser::Token::JUMPT, Parser::Token::VARIABLE, Parser::Token::NUMBER, Parser::Token::EOL },
		{ Parser::Token::SUB, Parser::Token::VARIABLE, Parser::Token::NUMBER, Parser::Token::VARIABLE, Parser::Token::EOL },
		{ Parser::Token::LT, Parser::Token::VARIABLE, Parser::Token::NUMBER, Parser::Token::VARIABLE, Parser::Token::EOL },
		{ Parser::Token::JUMPF, Parser::Token::VARIABLE, Parser::Token::NUMBER, Parser::Token::EOL },
		{ Parser::Token::ADD, Parser::Token::VARIABLE, Parser::Token::NUMBER, Parser::Token::VARIABLE, Parser::Token::EOL },
		{ Parser::Token::EQ, Parser::Token::VARIABLE, Parser::Token::NUMBER, Parser::Token::VARIABLE, Parser::Token::EOL },
		{ Parser::Token::JUMPF, Parser::Token::VARIABLE, Parser::Token::NUMBER, Parser::Token::EOL },
		{ Parser::Token::ASSIGN, Parser::Token::VARIABLE, Parser::Token::NUMBER, Parser::Token::EOL },
		{ Parser::Token::JUMP, Parser::Token::NUMBER, Parser::Token::EOL },
		{ Parser::Token::ASSIGN, Parser::Token::VARIABLE, Parser::Token::NUMBER, Parser::Token::EOL },
		{ Parser::Token::JUMP, Parser::Token::NUMBER, Parser::Token::EOL },
		{ Parser::Token::ASSIGN, Parser::Token::VARIABLE, Parser::Token::NUMBER, Parser::Token::EOL },
		{ Parser::Token::WRITE, Parser::Token::VARIABLE, Parser::Token::EOL },
		{ Parser::Token::NOP, Parser::Token::EOL }
	};

	std::vector<std::vector<Parser::Token>> t6{
		{ Parser::Token::READ, Parser::Token::VARIABLE, Parser::Token::EOL },
		{ Parser::Token::EOL },
		{ Parser::Token::ASSIGN, Parser::Token::VARIABLE, Parser::Token::NUMBER, Parser::Token::EOL },
		{ Parser::Token::NOP, Parser::Token::EOL }
	};

	REQUIRE(tokenize_file("tests/1.txt") == t1);
	// REQUIRE(tokenize_file("tests/2.txt") == t2);
	REQUIRE(tokenize_file("tests/3.txt") == t3);
	//REQUIRE(tokenize_file("tests/4.txt") == t4);
	//REQUIRE(tokenize_file("tests/3.txt") == t5);
	REQUIRE(tokenize_file("tests/6.txt") == t6);
}

TEST_CASE("Instruction tests", "[parser]")
{
	Parser l1("READ,variable");
	REQUIRE(l1.next() == Parser::Token::READ);
	REQUIRE(l1.next() == Parser::Token::VARIABLE);
	REQUIRE(l1.str() == "variable");

	Parser l2("+,vstup,2,vstup");
	REQUIRE(l2.next() == Parser::Token::ADD);
	REQUIRE(l2.next() == Parser::Token::VARIABLE);
	REQUIRE(l2.str() == "vstup");
	REQUIRE(l2.next() == Parser::Token::NUMBER);
	REQUIRE(l2.num() == 2);
	REQUIRE(l2.next() == Parser::Token::VARIABLE);
	REQUIRE(l2.str() == "vstup");

	Parser l3("=,status,-1");
	REQUIRE(l3.next() == Parser::Token::ASSIGN);
	REQUIRE(l3.next() == Parser::Token::VARIABLE);
	REQUIRE(l3.str() == "status");
	REQUIRE(l3.next() == Parser::Token::NUMBER);
	REQUIRE(l3.num() == -1);

	Parser l4("READ,9999variable");
	REQUIRE(l4.next() == Parser::Token::READ);
	REQUIRE(l4.next() == Parser::Token::NUMBER);
	REQUIRE(l4.num() == 9999);

	Parser l5("READ,variable9999");
	REQUIRE(l5.next() == Parser::Token::READ);
	REQUIRE(l5.next() == Parser::Token::VARIABLE);
	REQUIRE(l5.str() == "variable9999");
}

TEST_CASE("Functionality tests", "[interpreter]")
{
	Interpreter i1;
	REQUIRE(i1.execute("tests/read_write.txt") == Interpreter::Status::OK);
}

#else

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		std::cerr << "Usage: " << argv[0] << " <instruction_file>\n";
		return EXIT_FAILURE;
	}

	std::string filename(argv[1], 1024);
	Interpreter interp;
	
	if (!interp.loadFile(filename))
	{
		std::cerr << "File \"" << filename << "\" was not found\n";
		return EXIT_FAILURE;
	}

	Interpreter::Status status = interp.execute();
	switch (status)
	{
	case Interpreter::Status::OK:
		{
			std::cout << "OK!\n";
			break;
		}
	case Interpreter::Status::INVALID_INSTRUCTION:
		{
			std::cerr << "Line: " << interp.getLineNumber() << ", invalid instruction: \"" << interp.getErrorInfo() << "\"\n";
			break;
		}
	case Interpreter::Status::INVALID_OPERATOR:
		{
			// Error message handled internally
			break;
		}
	case Interpreter::Status::VARIABLE_DOESNT_EXIST:
		{
			std::cerr << "Line: " << interp.getLineNumber() << ", variable \"" << interp.getErrorInfo() << "\" does not exist\n";
			break;
		}
	case Interpreter::Status::INVALID_JUMP:
		{
			std::cerr << "Line: " << interp.getLineNumber() << ", invalid jump to line " << interp.getErrorInfo() << "\n";
			break;
		}
	}

	return status;
}

#endif // _TESTS