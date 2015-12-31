#include <iostream>
#include <fstream>
#include <cstdarg>

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
	if (yyin != nullptr)
		fclose(yyin);
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

int main(int argc, char** argv)
{
	std::string outputFileName = "out.asm";
	yyin = nullptr;
	if (argc == 2)
	{
		;
	}
	else if (argc == 3)
	{
		outputFileName = argv[2];
	}
	else
	{
		exitCode = 5;
		finalize(exitCode);
		return exitCode;
	}

	//yydebug = 1;
	yyin = fopen(argv[1], "r");
	if (yyin == nullptr)
	{
		exitCode = 5;
		finalize(exitCode);
		return exitCode;
	}

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
		if (pair.second->getType() != frontend::Symbol::Type::FUNCTION)
			continue;

		if (!static_cast<frontend::FunctionSymbol*>(pair.second)->isDefined())
		{
			exitCode = 3;
			finalize(exitCode);
			return exitCode;
		}
	}

	// Check presence of main
	frontend::Symbol* mainSymbol = context.globalSymbolTable()->findSymbol("main");
	if (mainSymbol == nullptr)
	{
		exitCode = 3;
		finalize(exitCode);
		return exitCode;
	}

	// It must be a function
	if (mainSymbol->getType() != frontend::Symbol::Type::MANGLING_LINK)
	{
		exitCode = 3;
		finalize(exitCode);
		return exitCode;
	}

	// Only one main has to be defined
	frontend::ManglingLinkSymbol* manglingLinkMainSymbol = static_cast<frontend::ManglingLinkSymbol*>(mainSymbol);
	if (manglingLinkMainSymbol->getFunctions().size() != 1)
	{
		exitCode = 3;
		finalize(exitCode);
		return exitCode;
	}

	// It has to always be int main(void)
	frontend::FunctionSymbol* funcMainSymbol = manglingLinkMainSymbol->getFunctions().front();
	if (funcMainSymbol->getReturnType() != frontend::Symbol::DataType::INT || funcMainSymbol->getParameters().size() != 0)
	{
		exitCode = 3;
		finalize(exitCode);
		return exitCode;
	}

	ir::Builder builder;
	program.generateIr(builder);

	std::ofstream outputFile(outputFileName, std::ios::trunc | std::ios::out);
	backend::ASMgenerator generator;
	generator.translateIR(builder);

	//std::cout << builder.codeText() << std::endl;
	try
	{
		outputFile << generator.getTargetCode() << std::endl;
	}
	catch (int e)
	{
		std::cerr << "Generated assemly does not fit into 1MB. Assumed code size is: " << e << "B, stack has 64kB\n";
		outputFile.close();
		exitCode = 4;
		finalize(exitCode);
		return exitCode;
	}
	outputFile.close();

	fclose(yyin);
	yylex_destroy(); // Fix memory leaks from flex, bison doesn't call this
	return 0;
}
