%option noyywrap

%{
#include <string>
#include <vector>

#include "frontend/ast.h"
#include "frontend/c_parser.h"

static char inputChar = '\0';
static std::string inputString = "";
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

\'                      { inputChar = '\0'; BEGIN(CHR); }
<CHR>\\\'               { inputChar = '\''; BEGIN(CHR_END); }
<CHR>\\\"               { inputChar = '\"'; BEGIN(CHR_END); }
<CHR>\\\\               { inputChar = '\\'; BEGIN(CHR_END); }
<CHR>\\t                { inputChar = '\t'; BEGIN(CHR_END); }
<CHR>\\n                { inputChar = '\n'; BEGIN(CHR_END); }
<CHR>[^'"\t\n\\]        { inputChar = yytext[0]; BEGIN(CHR_END); }
<CHR>.                  { yyterminate(); }
<CHR_END>\'             {
                            BEGIN(0);
                            yylval.charValue = inputChar;
                            return CHAR_LIT;
                        }

\"                      { inputString = ""; BEGIN(STR); }
<STR>\\\'               { inputString += '\''; }
<STR>\\\"               { inputString += '\"'; }
<STR>\\\\               { inputString += '\\'; }
<STR>\\t                { inputString += '\t'; }
<STR>\\n                { inputString += '\n'; }
<STR>([^\\"])+          { inputString += std::string(yytext, yyleng); }
<STR>\"                 {
                            BEGIN(0);
                            yylval.strValue = new std::string(inputString);
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
                            yylval.strValue = new std::string(yytext, yyleng);
                            return TYPE;
                        }
{digit}+                {
                            yylval.intValue = atoi(yytext);
                            return INT_LIT;
                        }
{letter}({alphanum})*   {
                            yylval.strValue = new std::string(yytext, yyleng);
                            return ID;
                        }

%%