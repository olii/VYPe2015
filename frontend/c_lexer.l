%option noyywrap

%{
#include <string>
#include <vector>

#include "frontend/ast.h"
#include "frontend/c_parser.h"

static char inputChar = '\0';
static std::string inputString = "";

extern void finalize(int exitCode);
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
<CHR>.                  { finalize(1); yyterminate(); }
<CHR_END>\'             {
							BEGIN(0);
							yylval.charValue = inputChar;
							return CHAR_LIT;
						}
<CHR_END>.              { finalize(1); yyterminate(); }

\"                      { inputString = ""; BEGIN(STR); }
<STR>\\\'               { inputString += "\\'"; }
<STR>\\\"               { inputString += "\\\""; }
<STR>\\\\               { inputString += "\\\\"; }
<STR>\\t                { inputString += "\\t"; }
<STR>\\n                { inputString += "\\n"; }
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
"&"                     { return BIT_AND; }
"|"                     { return BIT_OR; }
"~"                     { return BIT_NOT; }
if                      { return IF; }
else                    { return ELSE; }
while                   { return WHILE; }
for                     { return FOR; }
return                  { return RETURN; }
void                    { return VOID; }
unsigned                { return RESERVED; }
short                   { return RESERVED; }
break                   { return RESERVED; }
continue                { return RESERVED; }
int|char|string         {
							yylval.strValue = new std::string(yytext, yyleng);
							return TYPE;
						}
print|read_char|read_int|read_string|get_at|set_at|strcat {
							yylval.strValue = new std::string(yytext, yyleng);
							return BUILTIN;
						}
{digit}+                {
							yylval.intValue = atoi(yytext);
							return INT_LIT;
						}
{letter}({alphanum})*   {
							yylval.strValue = new std::string(yytext, yyleng);
							return ID;
						}
.                       { finalize(1); yyterminate(); }

%%
