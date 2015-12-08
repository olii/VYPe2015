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

int exitCode = 0;
frontend::Program program;
frontend::Context context;

void finalize(int _exitCode)
{
	exitCode = _exitCode;
	yylex_destroy(); // Fix memory leaks from flex, bison doesn't call this
}

void yyerror(const char *s, ...)
{
	va_list ap;
	va_start(ap, s);
	fprintf(stderr, "Error: ");
	vfprintf(stderr, s, ap);
	fprintf(stderr, "\n");
}

int main()
{
	//yydebug = 1;
	yyin = fopen("test.c", "r");
	int result = yyparse();
	if (result != 0)
	{
		if (exitCode == 0)
			exitCode = 2;
		finalize(exitCode);
		return exitCode;
	}

	// Check if there is no undefined but declared function
	for (const auto& pair : context.globalSymbolTable()->getAllSymbols())
	{
		if (pair.second->getType() != frontend::Symbol::FUNCTION)
			continue;

		if (!static_cast<frontend::FunctionSymbol*>(pair.second)->isDefined())
		{
			finalize(exitCode);
			return exitCode;
		}
	}

	// Check presence of main
	frontend::Symbol* mainSymbol = context.globalSymbolTable()->findSymbol("main");
	if (mainSymbol == nullptr)
	{
		finalize(exitCode);
		return exitCode;
	}

	// It must be a function
	if (mainSymbol->getType() != frontend::Symbol::Type::FUNCTION)
	{
		finalize(exitCode);
		return exitCode;
	}

	// It has to always be int main(void)
	frontend::FunctionSymbol* funcMainSymbol = static_cast<frontend::FunctionSymbol*>(mainSymbol);
	if (funcMainSymbol->getReturnType() != frontend::Symbol::DataType::INT || funcMainSymbol->getParameters().size() != 0)
	{
		finalize(exitCode);
		return exitCode;
	}

	ir::Builder builder;
	program.generateIr(builder);
    backend::ASMgenerator generator;
    generator.translateIR(builder);

	std::cout << builder.codeText() << std::endl;
	yylex_destroy(); // Fix memory leaks from flex, bison doesn't call this
	return 0;
}
