#include <iostream>

#include "frontend/ast.h"
#include "frontend/context.h"
#include "backend/asmgenerator.h"

// Have to be included last
#include "frontend/c_parser.h"

extern FILE* yyin;
extern int yyparse();
extern int yydebug;
extern int yylex_destroy();

frontend::Program program;
frontend::Context context;

void yyerror(const char* e)
{
	std::cerr << e << std::endl;
}

void terminate_compiler(int exit_code, const std::string& msg = "")
{
	if (msg != "")
		std::cerr << msg << std::endl;
	yylex_destroy(); // Fix memory leaks from flex, bison doesn't call this
	exit(exit_code);
}

int main()
{
	//yydebug = 1;
	yyin = fopen("test.c", "r");
	int result = yyparse();
	if (result != 0)
		terminate_compiler(3);

	// Check presence of main
	frontend::Symbol* mainSymbol = context.globalSymbolTable()->findSymbol("main");
	if (mainSymbol == nullptr)
		terminate_compiler(3);

	// It must be a function
	if (mainSymbol->getType() != frontend::Symbol::Type::FUNCTION)
		terminate_compiler(3);

	// It has to always be int main(void)
	frontend::FunctionSymbol* funcMainSymbol = static_cast<frontend::FunctionSymbol*>(mainSymbol);
	if (funcMainSymbol->getReturnType() != frontend::Symbol::DataType::INT || funcMainSymbol->getParameters().size() != 0)
		terminate_compiler(3);

	ir::Builder builder;
	program.generateIr(builder);
    backend::ASMgenerator generator;
    generator.translateIR(builder);

	std::cout << builder.codeText() << std::endl;
	yylex_destroy(); // Fix memory leaks from flex, bison doesn't call this
	return 0;
}
