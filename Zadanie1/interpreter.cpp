#include "interpreter.hpp"

#include <fstream>
#include <iostream>
#include <algorithm>


Interpreter::Interpreter()
{
	;
}


Interpreter::Status Interpreter::execute(const std::string &filename)
{
	std::ifstream in(filename);
	if (!in.is_open())
	{
		return Status::FILE_NOT_FOUND;
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
		return Status::NOTHING_TO_DO;
	}

	for (size_t i = 0; i < lines.size(); i++)
	{
		execute(lines.at(i), i);
	}

	return Status::OK;
}


Interpreter::Status Interpreter::execute(const std::string &line, size_t line_index = 0)
{
	Parser p(line);

	// First token is always assumed to be the instruction to execute
	Parser::Token t = p.next();
	if (t == Parser::Token::EOL)
	{
		return Status::NOTHING_TO_DO;
	}

	Status status;
	switch (t)
	{
	// 0 operators
	case Parser::Token::NOP: status = ins_nop(p); break;

	// 1 operator
	case Parser::Token::JUMP: status = ins_jump(p); break;
	case Parser::Token::READ: status = ins_read(p); break;
	case Parser::Token::WRITE: status = ins_write(p); break;

	// 2 operators
	case Parser::Token::ASSIGN: status = ins_assign(p); break;
	case Parser::Token::JUMPT: status = ins_jumpt(p); break;
	case Parser::Token::JUMPF: status = ins_jumpf(p); break;

	// 3 operators
	case Parser::Token::ADD: status = ins_add(p); break;
	case Parser::Token::SUB: status = ins_sub(p); break;
	case Parser::Token::MULTIPLY: status = ins_multiply(p); break;
	case Parser::Token::LT: status = ins_lt(p); break;
	case Parser::Token::GT: status = ins_gt(p); break;
	case Parser::Token::LTE: status = ins_lte(p); break;
	case Parser::Token::GTE: status = ins_gte(p); break;
	case Parser::Token::EQ: status = ins_eq(p); break;

	default:
		{
			status = Status::INVALID_INSTRUCTION;
			break;
		}
	}

	if (status != Status::OK)
	{
		std::cerr << "Error occured on line " << line_index + 1 << "\n";
		return status;
	}

	return Status::OK;
}

Interpreter::Status Interpreter::ins_nop(Parser &p)
{
	return Status::OK;
}


Interpreter::Status Interpreter::ins_jump(Parser &p)
{
	return Status::OK;
}


Interpreter::Status Interpreter::ins_read(Parser &p)
{

	return Status::OK;
}


Interpreter::Status Interpreter::ins_write(Parser &p)
{
	return Status::OK;
}


Interpreter::Status Interpreter::ins_assign(Parser &p)
{
	return Status::OK;
}


Interpreter::Status Interpreter::ins_jumpt(Parser &p)
{
	return Status::OK;
}


Interpreter::Status Interpreter::ins_jumpf(Parser &p)
{
	return Status::OK;
}


Interpreter::Status Interpreter::ins_add(Parser &p)
{
	return Status::OK;
}


Interpreter::Status Interpreter::ins_sub(Parser &p)
{
	return Status::OK;
}


Interpreter::Status Interpreter::ins_multiply(Parser &p)
{
	return Status::OK;
}


Interpreter::Status Interpreter::ins_lt(Parser &p)
{
	return Status::OK;
}


Interpreter::Status Interpreter::ins_gt(Parser &p)
{
	return Status::OK;
}


Interpreter::Status Interpreter::ins_lte(Parser &p)
{
	return Status::OK;
}


Interpreter::Status Interpreter::ins_gte(Parser &p)
{
	return Status::OK;
}


Interpreter::Status Interpreter::ins_eq(Parser &p)
{
	return Status::OK;
}