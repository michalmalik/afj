#pragma once

#include <string>
#include <vector>


class Interpreter
{
private:
	/*
	enum InstructionType
	{
		// 0 operators
		INS_NOP,

		// 1 operator
		INS_JUMP,
		INS_READ,
		INS_WRITE,

		// 2 operators
		INS_ASSIGN,
		INS_JUMPT,
		INS_JUMPF,

		// 3 operators
		INS_PLUS,
		INS_MINUS,
		INS_MULTIPLY,
		INS_DIVIDE,
		INS_LT,
		INS_GT,
		INS_LTE,
		INS_GTE,
		INS_EQ,
	};

	struct Instruction
	{
		size_t line_index;
		InstructionType type;
		
	};
	*/

	std::string m_filename;

public:
	enum Status
	{
		OK,
		FILE_NOT_FOUND,
		NOTHING_TO_DO
	};

	explicit Interpreter(std::string filename);

	Status execute();
};