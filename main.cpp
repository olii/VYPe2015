#include <iostream>
#include "frontend/c_parser.h"

extern int yyparse();
extern int yydebug;

void yyerror(const char* e)
{
    std::cerr << e << std::endl;
}

int main()
{
    yydebug = 1;
    yyparse();
    return 0;
}
