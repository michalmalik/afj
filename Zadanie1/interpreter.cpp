#include "interpreter.hpp"

#include <fstream>
#include <iostream>
#include <algorithm>


Interpreter::Interpreter()
{
	;
}


bool Interpreter::loadFile(const std::string &filename)
{
	std::ifstream in(filename);
	if (!in.is_open())
	{
		return false;
	}

	std::string line;
	while (std::getline(in, line))
	{
		if (!line.empty())
		{
			m_lines.push_back(line);
		}
	}

	in.close();

	return true;
}


bool Interpreter::loadLine(const std::string &line)
{
	m_lines.push_back(line);
	return true;
}


Interpreter::Status Interpreter::execute()
{
	while (m_line_index < m_lines.size())
	{
		const std::string &line = m_lines.at(m_line_index);

		const size_t index_before = m_line_index;
		Status status;
		if ((status = step(line)) != Status::OK)
		{
			return status;
		}

		// If the line index was altered by an instruction (JUMPs), don't increment
		// All JUMP instructions must ensure the jump is valid, so we dont do that here
		if (m_line_index == index_before)
		{
			m_line_index++;
		}
	}

	return Status::OK;
}


Interpreter::Status Interpreter::step(const std::string &line)
{
	Parser p(line);

	// First token is always assumed to be the instruction to execute
	Parser::Token t = p.next();
	if (t == Parser::Token::EOL)
	{
		return Status::OK;
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
			m_error_info = p.str();
			return Status::INVALID_INSTRUCTION;
		}
	}

	return status;
}


Interpreter::Status Interpreter::ins_nop(Parser &p)
{
	// Literally do nothing
	return Status::OK;
}


Interpreter::Status Interpreter::ins_jump(Parser &p)
{
	if (!expect(p, Parser::Token::NUMBER))
	{
		return Status::INVALID_OPERATOR;
	}

	return jump(p.num());
}


Interpreter::Status Interpreter::ins_read(Parser &p)
{
	if (!expect(p, Parser::Token::VARIABLE))
	{
		return Status::INVALID_OPERATOR;
	}

	int value;
	bool valid = false;

	do {
		std::cout << "Enter value for variable \"" << p.str() << "\": ";
		std::cin >> value;

		if (!(valid = std::cin.good()))
		{
			std::cout << "Invalid input\n";
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}
	} while (!valid);

	m_variables[p.str()] = value;
	return Status::OK;
}


Interpreter::Status Interpreter::ins_write(Parser &p)
{
	if (!expect(p, Parser::Token::VARIABLE))
	{
		return Status::INVALID_OPERATOR;
	}

	int value;
	if (!getVar(p.str(), value))
	{
		return Status::VARIABLE_DOESNT_EXIST;
	}

	std::cout << "Value of variable \"" << p.str() << "\": " << value << "\n";
	return Status::OK;
}


Interpreter::Status Interpreter::ins_assign(Parser &p)
{
	if (!expect(p, Parser::Token::VARIABLE))
	{
		return Status::INVALID_OPERATOR;
	}

	// Cant use ref here as it might disappear later
	std::string var = p.str();

	int value;
	Status status;
	if ((status = getValue(p, value)) != Status::OK)
	{
		return status;
	}

	m_variables[var] = value;

	return Status::OK;
}


Interpreter::Status Interpreter::ins_jumpt(Parser &p)
{
	int value;
	Status status;
	if ((status = getValue(p, value)) != Status::OK)
	{
		return status;
	}

	// Condition is false, don't jump
	if (!value)
	{
		return Status::OK;
	}

	if (!expect(p, Parser::Token::NUMBER))
	{
		return Status::INVALID_OPERATOR;
	}

	return jump(p.num());
}


Interpreter::Status Interpreter::ins_jumpf(Parser &p)
{
	int value;
	Status status;
	if ((status = getValue(p, value)) != Status::OK)
	{
		return status;
	}

	// Condition is true, don't jump
	if (value)
	{
		return Status::OK;
	}

	if (!expect(p, Parser::Token::NUMBER))
	{
		return Status::INVALID_OPERATOR;
	}

	return jump(p.num());
}


Interpreter::Status Interpreter::ins_add(Parser &p)
{
	int v1, v2;
	Status status;
	
	if ((status = getValue(p, v1)) != Status::OK)
	{
		return status;
	}

	if ((status = getValue(p, v2)) != Status::OK)
	{
		return status;
	}

	if (!expect(p, Parser::Token::VARIABLE))
	{
		return Status::INVALID_OPERATOR;
	}

	m_variables[p.str()] = v1 + v2;

	return Status::OK;
}


Interpreter::Status Interpreter::ins_sub(Parser &p)
{
	int v1, v2;
	Status status;

	if ((status = getValue(p, v1)) != Status::OK)
	{
		return status;
	}

	if ((status = getValue(p, v2)) != Status::OK)
	{
		return status;
	}

	if (!expect(p, Parser::Token::VARIABLE))
	{
		return Status::INVALID_OPERATOR;
	}

	m_variables[p.str()] = v1 - v2;

	return Status::OK;
}


Interpreter::Status Interpreter::ins_multiply(Parser &p)
{
	int v1, v2;
	Status status;

	if ((status = getValue(p, v1)) != Status::OK)
	{
		return status;
	}

	if ((status = getValue(p, v2)) != Status::OK)
	{
		return status;
	}

	if (!expect(p, Parser::Token::VARIABLE))
	{
		return Status::INVALID_OPERATOR;
	}

	m_variables[p.str()] = v1 * v2;

	return Status::OK;
}


Interpreter::Status Interpreter::ins_lt(Parser &p)
{
	int v1, v2;
	Status status;

	if ((status = getValue(p, v1)) != Status::OK)
	{
		return status;
	}

	if ((status = getValue(p, v2)) != Status::OK)
	{
		return status;
	}

	if (!expect(p, Parser::Token::VARIABLE))
	{
		return Status::INVALID_OPERATOR;
	}

	m_variables[p.str()] = v1 < v2;

	return Status::OK;
}


Interpreter::Status Interpreter::ins_gt(Parser &p)
{
	int v1, v2;
	Status status;

	if ((status = getValue(p, v1)) != Status::OK)
	{
		return status;
	}

	if ((status = getValue(p, v2)) != Status::OK)
	{
		return status;
	}

	if (!expect(p, Parser::Token::VARIABLE))
	{
		return Status::INVALID_OPERATOR;
	}

	m_variables[p.str()] = v1 > v2;

	return Status::OK;
}


Interpreter::Status Interpreter::ins_lte(Parser &p)
{
	int v1, v2;
	Status status;

	if ((status = getValue(p, v1)) != Status::OK)
	{
		return status;
	}

	if ((status = getValue(p, v2)) != Status::OK)
	{
		return status;
	}

	if (!expect(p, Parser::Token::VARIABLE))
	{
		return Status::INVALID_OPERATOR;
	}

	m_variables[p.str()] = v1 <= v2;

	return Status::OK;
}


Interpreter::Status Interpreter::ins_gte(Parser &p)
{
	int v1, v2;
	Status status;

	if ((status = getValue(p, v1)) != Status::OK)
	{
		return status;
	}

	if ((status = getValue(p, v2)) != Status::OK)
	{
		return status;
	}

	if (!expect(p, Parser::Token::VARIABLE))
	{
		return Status::INVALID_OPERATOR;
	}

	m_variables[p.str()] = v1 >= v2;

	return Status::OK;
}


Interpreter::Status Interpreter::ins_eq(Parser &p)
{
	int v1, v2;
	Status status;

	if ((status = getValue(p, v1)) != Status::OK)
	{
		return status;
	}

	if ((status = getValue(p, v2)) != Status::OK)
	{
		return status;
	}

	if (!expect(p, Parser::Token::VARIABLE))
	{
		return Status::INVALID_OPERATOR;
	}

	m_variables[p.str()] = v1 == v2;

	return Status::OK;
}


bool Interpreter::expect(Parser &p, Parser::Token t) const
{
	Parser::Token got = p.next();
	if (got != t)
	{
		std::cerr << "Line: " << getLineNumber() << ", expected: " << p.token_to_str(t) << ", got: " << p.token_to_str(got)
			<< " = " << (got == Parser::Token::NUMBER ? std::to_string(p.num()) : p.str()) << "\n";
		return false;
	}

	return true;
}


Interpreter::Status Interpreter::jump(int value)
{
	if (value <= 0 || value - 1 >= m_lines.size())
	{
		m_error_info = std::to_string(value);
		return Status::INVALID_JUMP;
	}

	m_line_index = value - 1;
	return Status::OK;
}


Interpreter::Status Interpreter::getValue(Parser &p, int &value)
{
	Parser::Token t = p.next();
	if (t == Parser::Token::VARIABLE)
	{
		if (!getVar(p.str(), value))
		{
			return Status::VARIABLE_DOESNT_EXIST;
		}
	}
	else if (t == Parser::Token::NUMBER)
	{
		value = p.num();
	}
	else
	{
		std::cerr << "Line: " << getLineNumber() << ", expected " << p.token_to_str(Parser::Token::VARIABLE)
			<< " or " << p.token_to_str(Parser::Token::NUMBER) << ", got: " << p.token_to_str(t) << " = \"" << p.str() << "\"\n";

		return Status::INVALID_OPERATOR;
	}

	return Status::OK;
}


bool Interpreter::getVar(const std::string &varname, int &value)
{
	auto var = m_variables.find(varname);
	if (var != m_variables.end())
	{
		value = var->second;
		return true;
	}
	else
	{
		m_error_info = varname;
		return false;
	}
}