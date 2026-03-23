/*****************************************************************************
 *   rules.l - Flex Rules for TIPS Language Subset
 *   MSU CSE 4714/6714 Capstone Project (Fall 2025)
 *   Author: Derek Willis
*****************************************************************************/
%option noyywrap nodefault nounput
%option yylineno

%{
#include "lexer.h"
%}

 /* Macros */
DIG    [0-9]
YEAR   {DIG}{DIG}{DIG}{DIG}
MONTH_DAY  {DIG}{DIG}
QUOTE  ["]


%%
 /* DEFINE YOUR RULES STARTING HERE */
[ \t\n\r]+        {  }

 /* Keywords */
BEGIN           { return TOK_BEGIN; }
ELSE            { return ELSE; }
END             { return END; }
IF              { return IF; }
PROGRAM         { return PROGRAM; }
READ            { return READ; }
THEN            { return THEN; }
VAR             { return VAR; }
WHILE           { return WHILE; }
WRITE           { return WRITE; }
"CUSTOM KEYWORD"          { return CUSTOM; }

 /* Datatype Specifiers */
INTEGER         { return INTEGER; }
REAL            { return REAL; }
STRING          { return STRINGLIT; }

 /* Punctuation */
"("             { return OPENPAREN; }
")"             { return CLOSEPAREN; }
";"             { return SEMICOLON; }
":"             { return COLON; }

 /* Operators */
"+"             { return PLUS; }
"-"             { return MINUS; }
"*"             { return MULTIPLY; }
"/"             { return DIVIDE; }
":="             { return ASSIGN; }
"=="            { return EQUALTO; }
"<"             { return LESSTHAN; }
">"             { return GREATERTHAN; }
"!="            { return NOTEQUALTO; }
"%"             { return MOD; }
"!"             { return TOK_NOT; }
"||"            { return TOK_OR; }
"&&"            { return TOK_AND; }


 /* Identifiers 
 Must start with a capital letter [A-Z]
 Then may contain only capital letters or digits [A-Z0-9]*
 If the IDENT is longer than 8 characters, return UNKNOWN*/

[A-Z][A-Z0-9]*  { 
                    if (yyleng > 8) {
                        return UNKNOWN;
                    } else {
                        return IDENT; 
                    }
                }

[0-9]*            { return INTLIT; }
[0-9]*"."[0-9]+  { return FLOATLIT; }
[']([^'])*[']   { return STRINGLIT; }

 /* End-of-file */
<<EOF>>         { return TOK_EOF; }

 /* Anything else → unknown */
.               { return UNKNOWN; }

%%