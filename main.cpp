#include <iostream>

#include "frontend/ast.h"
#include "frontend/context.h"
#include "backend/asmgenerator.h"

// Have to be included last
#include "frontend/c_parser.h"

extern FILE* yyin;
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
	//yydebug = 1;
	yyin = fopen("test.c", "r");
	int result = yyparse();
	std::cout << result << std::endl;
	if (result != 0)
		return 1;

	// Check presence of main
	frontend::Symbol* mainSymbol = context.globalSymbolTable()->findSymbol("main");
	if (mainSymbol == nullptr)
		return 2;

	// It must be a function
	if (mainSymbol->getType() != frontend::Symbol::Type::FUNCTION)
		return 2;

	// It has to always be int main(void)
	frontend::FunctionSymbol* funcMainSymbol = static_cast<frontend::FunctionSymbol*>(mainSymbol);
	if (funcMainSymbol->getReturnType() != frontend::Symbol::DataType::INT || funcMainSymbol->getParameters().size() != 0)
		return 2;

	ir::Builder builder;
	program.generateIr(builder);
    backend::ASMgenerator generator;
    generator.translateIR(builder);

	std::cout << builder.codeText() << std::endl;
	return 0;
}
