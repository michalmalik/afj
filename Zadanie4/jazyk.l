%option noyywrap

%{
	#include <string>
	#include "parser.hpp"

	#define TOKEN(t) (yylval.token = t)
%}

%%
\*			{ return TOKEN(TITERATION); }
\|			{ return TOKEN(TUNION); }
\(			{ return TOKEN(TLEFT_PAR); }
\)			{ return TOKEN(TRIGHT_PAR); }
[a-zA-Z ]?	{
	yylval.string = new std::string(yytext, yyleng);
	return TCHARACTER;
}
%%