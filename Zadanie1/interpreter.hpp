#pragma once

#include "parser.hpp"

#include <string>
#include <vector>


class Interpreter
{
public:
	enum Status
	{
		OK,
		FILE_NOT_FOUND,
		NOTHING_TO_DO,
		INVALID_INSTRUCTION
	};

	explicit Interpreter();

	Status execute(const std::string &filename);
	Status execute(const std::string &line, size_t line_index);

private:
	size_t m_line_index;

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
};