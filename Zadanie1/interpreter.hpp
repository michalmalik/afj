#pragma once

#include "lexer.hpp"

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
	Status ins_nop(Lexer &p);

	// 1 operator
	Status ins_jump(Lexer &p);
	Status ins_read(Lexer &p);
	Status ins_write(Lexer &p);

	// 2 operators
	Status ins_assign(Lexer &p);
	Status ins_jumpt(Lexer &p);
	Status ins_jumpf(Lexer &p);

	// 3 operators
	Status ins_add(Lexer &p);
	Status ins_sub(Lexer &p);
	Status ins_multiply(Lexer &p);
	Status ins_lt(Lexer &p);
	Status ins_gt(Lexer &p);
	Status ins_lte(Lexer &p);
	Status ins_gte(Lexer &p);
	Status ins_eq(Lexer &p);

	bool expect(Lexer &p, Lexer::Token t) const;
	Status jump(int value);
	Status getValue(Lexer &p, int &value);

public:
	bool getVar(const std::string &varname, int &value);
};