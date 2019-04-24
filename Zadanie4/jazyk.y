%{
	#include <iostream>
	#include <string>

	#include "semanticanalysis.hpp"

	SemanticAnalysis *sem = new SemanticAnalysis();

	extern int yylex();
	void yyerror(const char *s) { printf("ERROR: %s\n", s); }
%}

%union {
	std::string *string;
	int token;
}

%token <string> TCHARACTER
%token <token> TITERATION TLEFT_PAR TRIGHT_PAR TUNION

%type <string> Base
%type <string> Factor
%type <string> Term
%type <string> Regex

%%

Regex :	Term
			{
				$$ = new std::string($1->c_str());
				// std::cout << "regex -> " << $$->c_str() << "\n";
			}
	  | Term TUNION Regex
			{
				// std::cout << "UNION: regex -> " << $1->c_str() << " | " << $3->c_str() << "\n";
				sem->buildUnion(*$1, *$3);
				$1->append("|" + std::string($3->c_str()));
				sem->reduceExpression(*$1);
			}
	  ;

Term : Factor
			{
				$$ = new std::string($1->c_str());
				// std::cout << "term -> " << $$->c_str() << "\n"; 
			}
	 | Factor Term
			{
				// std::cout << "CONCAT: term -> " << $1->c_str() << " + " << $2->c_str() << "\n";
				sem->buildConcat(*$1, *$2);
				$1->append($2->c_str());
				sem->reduceExpression(*$1);
			}
	 ;

Factor : Base
			{
				$$ = new std::string($1->c_str());
				// std::cout << "factor -> " << $1->c_str() << "\n";
			}
	   | Base TITERATION
			{
				// std::cout << "ITER: factor -> " << $1->c_str() << "*\n";
				sem->buildIteration(*$1);
				$1->append("*");
				sem->reduceExpression(*$1);
			}
	   ;

Base : TCHARACTER
			{
				// std::cout << "base -> " << $1->c_str() << "\n";
				sem->buildSymbol(std::string($1->c_str()));
			}
	 | TLEFT_PAR Regex TRIGHT_PAR
			{
				$$ = new std::string("(" + std::string($2->c_str()) + ")");
				// std::cout << "base -> ( " << $2->c_str() << " )\n";
				sem->reduceExpression(*$$);
			}
	 ;

%%