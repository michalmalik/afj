#pragma once

#include "parser.hpp"

#include <string>
#include <vector>
#include <map>


class Interpreter
{
public:
	enum Status
	{
		OK,
		INVALID_INSTRUCTION,
		INVALID_OPERATOR,
		VARIABLE_DOESNT_EXIST,
		INVALID_JUMP
	};

	explicit Interpreter();

	bool loadFile(const std::string &filename);
	bool loadLine(const std::string &line);
	Status execute();

	const std::string &getErrorInfo() const { return m_error_info; }
	size_t getLineNumber() const { return m_line_index + 1; }

private:
	std::vector<std::string> m_lines;

	std::string m_error_info;
	size_t m_line_index{ 0 };

	std::map<std::string, int> m_variables;

	Status step(const std::string &line);

	// 0 operators
	Status ins_nop(Parser &p);

	// 1 operator
	Status ins_jump(Parser &p);
	Status ins_read(Parser &p);
	Status ins_write(Parser &p);

	// 2 operators
	Status ins_assign(Parser &p);
	Status ins_jumpt(Parser &p);
	Status ins_jumpf(Parser &p);

	// 3 operators
	Status ins_add(Parser &p);
	Status ins_sub(Parser &p);
	Status ins_multiply(Parser &p);
	Status ins_lt(Parser &p);
	Status ins_gt(Parser &p);
	Status ins_lte(Parser &p);
	Status ins_gte(Parser &p);
	Status ins_eq(Parser &p);

	bool expect(Parser &p, Parser::Token t) const;
	Status jump(int value);
	Status getValue(Parser &p, int &value);

public:
	bool getVar(const std::string &varname, int &value);
};