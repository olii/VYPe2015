%option noyywrap

%{
#include <string>
#include "c_parser.h"
%}

digit       [0-9]
letter      [a-zA-Z_]
alphanum    [a-zA-Z0-9_]
newline     \n
whitespace  [ \r\n\t]

%x BLOCK_COMMENT
%x CHR
%x CHR_END
%x STR

%%

({whitespace})+         ;
\/\/(.*)                ;

\/\*                    { BEGIN(BLOCK_COMMENT); }
<BLOCK_COMMENT>\*\/     { BEGIN(0); }
<BLOCK_COMMENT>.        ;

\'                      { BEGIN(CHR); }
<CHR>\\\'               { BEGIN(CHR_END); }
<CHR>\\\"               { BEGIN(CHR_END); }
<CHR>\\\\               { BEGIN(CHR_END); }
<CHR>\\t                { BEGIN(CHR_END); }
<CHR>\\n                { BEGIN(CHR_END); }
<CHR>[^'"\t\n\\]        { BEGIN(CHR_END); }
<CHR>.                  ;
<CHR_END>\'             {
                            BEGIN(0);
                            return CHAR_LIT;
                        }

\"                      { BEGIN(STR); }
<STR>\\\'               ;
<STR>\\\"               ;
<STR>\\\\               ;
<STR>\\t                ;
<STR>\\n                ;
<STR>([^\\"])+          ;
<STR>\"                 {
                            BEGIN(0);
                            return STRING_LIT;
                        }

","                     { return COMMA; }
";"                     { return SEMICOLON; }
"="                     { return ASSIGN; }
"("                     { return LEFT_PAREN; }
")"                     { return RIGHT_PAREN; }
"{"                     { return LEFT_CBRACE; }
"}"                     { return RIGHT_CBRACE; }
"+"                     { return PLUS; }
"-"                     { return MINUS; }
"*"                     { return MULTIPLY; }
"/"                     { return DIVIDE; }
"%"                     { return MODULO; }
"!"                     { return NOT; }
"<"                     { return LESS; }
"<="                    { return LESS_EQUAL; }
">"                     { return GREATER; }
">="                    { return GREATER_EQUAL; }
"=="                    { return EQUAL; }
"!="                    { return NOT_EQUAL; }
"&&"                    { return AND; }
"||"                    { return OR; }
if                      { return IF; }
else                    { return ELSE; }
while                   { return WHILE; }
return                  { return RETURN; }
void                    { return VOID; }
int|char|string         {
                            return TYPE;
                        }
{digit}+                {
                            return INT_LIT;
                        }
{letter}({alphanum})*   {
                            return ID;
                        }

%%
