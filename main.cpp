#include <iostream>

#include "frontend/ast.h"
#include "frontend/context.h"

// Have to be included last
#include "frontend/c_parser.h"

extern int yyparse();
extern int yydebug;

frontend::Program program;
frontend::Context context;

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
