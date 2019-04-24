#pragma once

#include <string>
#include <vector>
#include <map>

#include "regexp.hpp"


class SemanticAnalysis
{
public:
	SemanticAnalysis() = default;

	const std::vector<RegExp> &getExpressions() const { return m_expressions; }
	RegExp &getFinal() { return m_expressions.at(m_expressions.size() - 1); }

	const std::vector<std::string> &getReducedExps() const { return m_reduced; }
	const std::map<std::string, size_t> &getExpressionTable() const { return m_reduce_table; }

	void buildConcat(const std::string &lhs, const std::string &rhs);
	void buildUnion(const std::string &lhs, const std::string &rhs);
	void buildIteration(const std::string &lhs);
	void buildSymbol(const std::string &s);

	bool reduceExpression(const std::string &exp);

private:
	bool isInPars(const std::string &exp) const;
	size_t reduce(const std::string &exp) const { return m_reduce_table.at(exp); }
	void insertReduced(const std::string &exp);

	std::vector<RegExp> m_expressions;

	// Maps strings into m_expressions
	size_t m_reduce_counter{ 0 };
	std::map<std::string, size_t> m_reduce_table;

	std::set<std::string> m_reduced_check;
	std::vector<std::string> m_reduced;
};