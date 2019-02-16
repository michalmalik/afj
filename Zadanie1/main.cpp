#include "interpreter.hpp"

#include <iostream>


#ifndef _TESTS

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		std::cerr << "Usage: " << argv[0] << " <instruction_file>\n";
		return EXIT_FAILURE;
	}

	std::string filename(argv[1]);
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

#else
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

TEST_CASE("Corner case tests", "[parser]")
{
	Parser l1("420");
	REQUIRE(l1.next() == Parser::Token::NUMBER);
	REQUIRE(l1.num() == 420);

	Parser l2("variable");
	REQUIRE(l2.next() == Parser::Token::VARIABLE);
	REQUIRE(l2.str() == "variable");

	Parser l3("-69");
	REQUIRE(l3.next() == Parser::Token::NUMBER);
	REQUIRE(l3.num() == -69);

	Parser l4("READ,9999variable");
	REQUIRE(l4.next() == Parser::Token::READ);
	REQUIRE(l4.next() == Parser::Token::NUMBER);
	REQUIRE(l4.num() == 9999);

	Parser l5("READ,variable9999");
	REQUIRE(l5.next() == Parser::Token::READ);
	REQUIRE(l5.next() == Parser::Token::VARIABLE);
	REQUIRE(l5.str() == "variable9999");

	Parser l6("READ");
	REQUIRE(l6.next() == Parser::Token::READ);
	REQUIRE(l6.next() == Parser::Token::EOL);

	Parser l7("READ,");
	REQUIRE(l7.next() == Parser::Token::READ);
	REQUIRE(l7.next() == Parser::Token::EOL);

	Parser l8(",READ");
	REQUIRE(l8.next() == Parser::Token::EOL);

	Parser l9("READ,,,,,,,");
	REQUIRE(l9.next() == Parser::Token::READ);
	REQUIRE(l9.next() == Parser::Token::EOL);

	Parser l10(",,,,,,,READ,,,,,,,");
	REQUIRE(l10.next() == Parser::Token::EOL);

	Parser l11("    READ,x    ");
	REQUIRE(l11.next() == Parser::Token::READ);
	REQUIRE(l11.next() == Parser::Token::VARIABLE);
	REQUIRE(l11.next() == Parser::Token::EOL);

	Parser l12("    READ,  x    ");
	REQUIRE(l12.next() == Parser::Token::READ);
	REQUIRE(l12.next() == Parser::Token::VARIABLE);
	REQUIRE(l12.next() == Parser::Token::EOL);

	Parser l13("    READ   ,x    ");
	REQUIRE(l13.next() == Parser::Token::READ);
	REQUIRE(l13.next() == Parser::Token::VARIABLE);
	REQUIRE(l13.next() == Parser::Token::EOL);

	Parser l14("    READ   ,    x    ");
	REQUIRE(l14.next() == Parser::Token::READ);
	REQUIRE(l14.next() == Parser::Token::VARIABLE);
	REQUIRE(l14.next() == Parser::Token::EOL);

	Parser l15("%(#*#%&*@($@#()$@$(%(#*^(*#(^&#&^!_)$(@)%(^*&)(!(%");
	REQUIRE(l15.next() == Parser::Token::STRING);
}

TEST_CASE("= tests", "[interpreter]")
{
	Interpreter interp;

	interp.loadLine("=,i,10");
	interp.loadLine("=,j,i");
	REQUIRE(interp.execute() == Interpreter::Status::OK);

	int i, j;
	REQUIRE(interp.getVar("i", i));
	REQUIRE(interp.getVar("j", j));
	REQUIRE(i == j);
}

TEST_CASE("+ tests", "[interpreter]")
{
	Interpreter interp;

	interp.loadLine("=,i,10");
	interp.loadLine("+,10,i,j");
	interp.loadLine("+,j,10,k");
	interp.loadLine("+,k,j,l");
	interp.loadLine("+,i,l,m");
	interp.loadLine("+,i,-5,n");
	interp.loadLine("+,-5,i,o");
	interp.loadLine("+,-5,-5,p");
	interp.loadLine("=,q,100");
	interp.loadLine("+,q,q,q");
	REQUIRE(interp.execute() == Interpreter::Status::OK);

	int i, j, k, l, m, n, o, p, q;
	REQUIRE(interp.getVar("i", i));
	REQUIRE(interp.getVar("j", j));
	REQUIRE(interp.getVar("k", k));
	REQUIRE(interp.getVar("l", l));
	REQUIRE(interp.getVar("m", m));
	REQUIRE(interp.getVar("n", n));
	REQUIRE(interp.getVar("o", o));
	REQUIRE(interp.getVar("p", p));
	REQUIRE(interp.getVar("q", q));
	REQUIRE(i == 10);
	REQUIRE(j == 20);
	REQUIRE(k == 30);
	REQUIRE(l == 50);
	REQUIRE(m == 60);
	REQUIRE(n == 5);
	REQUIRE(n == o);
	REQUIRE(p == -10);
	REQUIRE(q == 200);
}

TEST_CASE("- tests", "[interpreter]")
{
	Interpreter interp;

	interp.loadLine("=,i,10");
	interp.loadLine("-,10,i,j");
	interp.loadLine("-,j,10,k");
	interp.loadLine("-,k,j,l");
	interp.loadLine("-,i,l,m");
	interp.loadLine("-,i,-5,n");
	interp.loadLine("-,-5,i,o");
	interp.loadLine("-,-5,-5,p");
	REQUIRE(interp.execute() == Interpreter::Status::OK);

	int i, j, k, l, m, n, o, p;
	REQUIRE(interp.getVar("i", i));
	REQUIRE(interp.getVar("j", j));
	REQUIRE(interp.getVar("k", k));
	REQUIRE(interp.getVar("l", l));
	REQUIRE(interp.getVar("m", m));
	REQUIRE(interp.getVar("n", n));
	REQUIRE(interp.getVar("o", o));
	REQUIRE(interp.getVar("p", p));
	REQUIRE(i == 10);
	REQUIRE(j == 0);
	REQUIRE(k == -10);
	REQUIRE(l == -10);
	REQUIRE(m == 20);
	REQUIRE(n == 15);
	REQUIRE(o == -15);
	REQUIRE(p == 0);
}

TEST_CASE("* tests", "[interpreter]")
{
	Interpreter interp;

	interp.loadLine("=,i,10");
	interp.loadLine("*,10,i,j");
	interp.loadLine("*,j,10,k");
	interp.loadLine("*,k,j,l");
	interp.loadLine("*,i,l,m");
	interp.loadLine("*,i,-5,n");
	interp.loadLine("*,-5,i,o");
	interp.loadLine("*,-5,-5,p");
	REQUIRE(interp.execute() == Interpreter::Status::OK);

	int i, j, k, l, m, n, o, p;
	REQUIRE(interp.getVar("i", i));
	REQUIRE(interp.getVar("j", j));
	REQUIRE(interp.getVar("k", k));
	REQUIRE(interp.getVar("l", l));
	REQUIRE(interp.getVar("m", m));
	REQUIRE(interp.getVar("n", n));
	REQUIRE(interp.getVar("o", o));
	REQUIRE(interp.getVar("p", p));
	REQUIRE(i == 10);
	REQUIRE(j == 100);
	REQUIRE(k == 1000);
	REQUIRE(l == 100000);
	REQUIRE(m == 1000000);
	REQUIRE(n == -50);
	REQUIRE(o == -50);
	REQUIRE(p == 25);
}

TEST_CASE("< tests", "[interpreter]")
{
	Interpreter interp;

	interp.loadLine("=,i,10");
	interp.loadLine("<,i,20,j");
	interp.loadLine("<,20,i,k");
	interp.loadLine("<,30,100,l");
	interp.loadLine("<,k,j,m");
	interp.loadLine("<,-5,i,n");
	interp.loadLine("<,i,-5,o");
	interp.loadLine("<,-5,-10,p");
	REQUIRE(interp.execute() == Interpreter::Status::OK);

	int i, j, k, l, m, n, o, p;
	REQUIRE(interp.getVar("i", i));
	REQUIRE(interp.getVar("j", j));
	REQUIRE(interp.getVar("k", k));
	REQUIRE(interp.getVar("l", l));
	REQUIRE(interp.getVar("m", m));
	REQUIRE(interp.getVar("n", n));
	REQUIRE(interp.getVar("o", o));
	REQUIRE(interp.getVar("p", p));
	REQUIRE(i == 10);
	REQUIRE(j == 1);
	REQUIRE(k == 0);
	REQUIRE(l == 1);
	REQUIRE(m == 1);
	REQUIRE(n == 1);
	REQUIRE(o == 0);
	REQUIRE(p == 0);
}

TEST_CASE("> tests", "[interpreter]")
{
	Interpreter interp;

	interp.loadLine("=,i,10");
	interp.loadLine(">,i,20,j");
	interp.loadLine(">,20,i,k");
	interp.loadLine(">,30,100,l");
	interp.loadLine(">,k,j,m");
	interp.loadLine(">,-5,i,n");
	interp.loadLine(">,i,-5,o");
	interp.loadLine(">,-5,-10,p");
	REQUIRE(interp.execute() == Interpreter::Status::OK);

	int i, j, k, l, m, n, o, p;
	REQUIRE(interp.getVar("i", i));
	REQUIRE(interp.getVar("j", j));
	REQUIRE(interp.getVar("k", k));
	REQUIRE(interp.getVar("l", l));
	REQUIRE(interp.getVar("m", m));
	REQUIRE(interp.getVar("n", n));
	REQUIRE(interp.getVar("o", o));
	REQUIRE(interp.getVar("p", p));
	REQUIRE(i == 10);
	REQUIRE(j == 0);
	REQUIRE(k == 1);
	REQUIRE(l == 0);
	REQUIRE(m == 1);
	REQUIRE(n == 0);
	REQUIRE(o == 1);
	REQUIRE(p == 1);
}

TEST_CASE("<= tests", "[interpreter]")
{
	Interpreter interp;

	interp.loadLine("=,i,10");
	interp.loadLine("<=,i,10,j");
	interp.loadLine("<=,20,i,k");
	interp.loadLine("<=,100,100,l");
	interp.loadLine("<=,k,j,m");
	interp.loadLine("<=,-5,i,n");
	interp.loadLine("<=,i,-5,o");
	interp.loadLine("<=,-5,-10,p");
	REQUIRE(interp.execute() == Interpreter::Status::OK);

	int i, j, k, l, m, n, o, p;
	REQUIRE(interp.getVar("i", i));
	REQUIRE(interp.getVar("j", j));
	REQUIRE(interp.getVar("k", k));
	REQUIRE(interp.getVar("l", l));
	REQUIRE(interp.getVar("m", m));
	REQUIRE(interp.getVar("n", n));
	REQUIRE(interp.getVar("o", o));
	REQUIRE(interp.getVar("p", p));
	REQUIRE(i == 10);
	REQUIRE(j == 1);
	REQUIRE(k == 0);
	REQUIRE(l == 1);
	REQUIRE(m == 1);
	REQUIRE(n == 1);
	REQUIRE(o == 0);
	REQUIRE(p == 0);
}

TEST_CASE(">= tests", "[interpreter]")
{
	Interpreter interp;

	interp.loadLine("=,i,10");
	interp.loadLine(">=,i,10,j");
	interp.loadLine(">=,20,i,k");
	interp.loadLine(">=,100,100,l");
	interp.loadLine(">=,k,j,m");
	interp.loadLine(">=,-5,i,n");
	interp.loadLine(">=,i,-5,o");
	interp.loadLine(">=,-5,-10,p");
	REQUIRE(interp.execute() == Interpreter::Status::OK);

	int i, j, k, l, m, n, o, p;
	REQUIRE(interp.getVar("i", i));
	REQUIRE(interp.getVar("j", j));
	REQUIRE(interp.getVar("k", k));
	REQUIRE(interp.getVar("l", l));
	REQUIRE(interp.getVar("m", m));
	REQUIRE(interp.getVar("n", n));
	REQUIRE(interp.getVar("o", o));
	REQUIRE(interp.getVar("p", p));
	REQUIRE(i == 10);
	REQUIRE(j == 1);
	REQUIRE(k == 1);
	REQUIRE(l == 1);
	REQUIRE(m == 1);
	REQUIRE(n == 0);
	REQUIRE(o == 1);
	REQUIRE(p == 1);
}

TEST_CASE("== tests", "[interpreter]")
{
	Interpreter interp;

	interp.loadLine("=,i,10");
	interp.loadLine("==,i,10,j");
	interp.loadLine("==,20,i,k");
	interp.loadLine("==,100,100,l");
	interp.loadLine("==,k,j,m");
	interp.loadLine("==,-5,i,n");
	interp.loadLine("==,i,-5,o");
	interp.loadLine("==,-5,-10,p");
	REQUIRE(interp.execute() == Interpreter::Status::OK);

	int i, j, k, l, m, n, o, p;
	REQUIRE(interp.getVar("i", i));
	REQUIRE(interp.getVar("j", j));
	REQUIRE(interp.getVar("k", k));
	REQUIRE(interp.getVar("l", l));
	REQUIRE(interp.getVar("m", m));
	REQUIRE(interp.getVar("n", n));
	REQUIRE(interp.getVar("o", o));
	REQUIRE(interp.getVar("p", p));
	REQUIRE(i == 10);
	REQUIRE(j == 1);
	REQUIRE(k == 0);
	REQUIRE(l == 1);
	REQUIRE(m == 0);
	REQUIRE(n == 0);
	REQUIRE(o == 0);
	REQUIRE(p == 0);
}

TEST_CASE("Jump tests", "[interpreter]")
{
	Interpreter i1;
	i1.loadFile("tests/invalid_jump_1.txt");
	REQUIRE(i1.execute() == Interpreter::Status::INVALID_JUMP);
	REQUIRE(i1.getLineNumber() == 2);
	REQUIRE(i1.getErrorInfo() == "10");

	Interpreter i12;
	i12.loadFile("tests/invalid_jump_2.txt");
	REQUIRE(i12.execute() == Interpreter::Status::INVALID_JUMP);
	REQUIRE(i12.getLineNumber() == 2);
	REQUIRE(i12.getErrorInfo() == "0");

	Interpreter i13;
	i13.loadFile("tests/invalid_jump_3.txt");
	REQUIRE(i13.execute() == Interpreter::Status::INVALID_JUMP);
	REQUIRE(i13.getLineNumber() == 2);
	REQUIRE(i13.getErrorInfo() == "-100");

	Interpreter i2;
	i2.loadFile("tests/invalid_jumpt.txt");
	REQUIRE(i2.execute() == Interpreter::Status::INVALID_JUMP);
	REQUIRE(i2.getLineNumber() == 3);
	REQUIRE(i2.getErrorInfo() == "10");

	Interpreter i3;
	i3.loadFile("tests/invalid_jumpf.txt");
	REQUIRE(i3.execute() == Interpreter::Status::INVALID_JUMP);
	REQUIRE(i3.getLineNumber() == 4);
	REQUIRE(i3.getErrorInfo() == "10");

	Interpreter i4;
	i4.loadFile("tests/jump.txt");
	REQUIRE(i4.execute() == Interpreter::Status::OK);
	int i, x;
	REQUIRE(i4.getVar("i", i));
	REQUIRE(!i4.getVar("x", x));
	REQUIRE(i == 20);

	Interpreter i5;
	i5.loadFile("tests/jumpt.txt");
	REQUIRE(i5.execute() == Interpreter::Status::OK);
	REQUIRE(i5.getVar("i", i));
	REQUIRE(!i5.getVar("x", x));
	REQUIRE(i == 20);

	Interpreter i6;
	i6.loadFile("tests/jumpf.txt");
	REQUIRE(i6.execute() == Interpreter::Status::OK);
	REQUIRE(i6.getVar("i", i));
	REQUIRE(!i6.getVar("x", x));
	REQUIRE(i == 20);
}

TEST_CASE("Variable tests", "[interpreter]")
{
	Interpreter i1;
	i1.loadFile("tests/4_test.txt");
	REQUIRE(i1.execute() == Interpreter::Status::VARIABLE_DOESNT_EXIST);
	REQUIRE(i1.getLineNumber() == 15);
	REQUIRE(i1.getErrorInfo() == "statusq");

	Interpreter i2;
	i2.loadFile("tests/5_test.txt");
	REQUIRE(i2.execute() == Interpreter::Status::VARIABLE_DOESNT_EXIST);
	REQUIRE(i2.getLineNumber() == 3);
	REQUIRE(i2.getErrorInfo() == "vacsi");

	Interpreter i3;
	i3.loadLine("=,i,10");
	i3.loadLine("==,j,i,k");
	REQUIRE(i3.execute() == Interpreter::Status::VARIABLE_DOESNT_EXIST);
	REQUIRE(i3.getLineNumber() == 2);
	REQUIRE(i3.getErrorInfo() == "j");

	Interpreter i4;
	i4.loadLine("=,i,10");
	i4.loadLine("==,i,j,k");
	REQUIRE(i4.execute() == Interpreter::Status::VARIABLE_DOESNT_EXIST);
	REQUIRE(i4.getLineNumber() == 2);
	REQUIRE(i4.getErrorInfo() == "j");
}

TEST_CASE("Instruction tests", "[interpreter]")
{
	/*
	START: very obvious tests for invalid forms of instructions
	*/
	Interpreter i1;
	i1.loadLine("READ,25");
	REQUIRE(i1.execute() == Interpreter::Status::INVALID_OPERATOR);

	Interpreter i2;
	i2.loadLine("WRITE,25");
	REQUIRE(i2.execute() == Interpreter::Status::INVALID_OPERATOR);

	Interpreter i3;
	i3.loadLine("=,10,50");
	REQUIRE(i3.execute() == Interpreter::Status::INVALID_OPERATOR);

	Interpreter i4;
	i4.loadLine("==,10,50,20");
	REQUIRE(i4.execute() == Interpreter::Status::INVALID_OPERATOR);

	Interpreter i5;
	i5.loadLine("=,i,10");
	i5.loadLine("JUMP,i");
	REQUIRE(i5.execute() == Interpreter::Status::INVALID_OPERATOR);
	/*
	END: very obvious tests
	*/
}

#endif // _TESTS