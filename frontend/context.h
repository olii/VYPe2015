/**
 * Project VYPe 2015/2016
 * Marek Milkovič (xmilko01) & Oliver Nemček (xnemce03)
 **/

#ifndef FRONTEND_CONTEXT_H
#define FRONTEND_CONTEXT_H

#include <vector>

#include "frontend/symbol_table.h"

namespace frontend {

class Context
{
public:
	Context();
	Context(const Context&) = delete;
	~Context();

	using StackType = std::vector<SymbolTable*>;

	void newSymbolTable();
	void popSymbolTable();

	SymbolTable* globalSymbolTable();
	SymbolTable* currentSymbolTable();

	FunctionSymbol* getCurrentFunction() const;
	void setCurrentFunction(FunctionSymbol* currentFunction);

	Symbol::DataType getCurrentDeclaredVarType() const;
	void setCurrentDeclaredVarType(Symbol::DataType dataType);

	Symbol* findSymbol(const std::string& name);
	const Symbol* findSymbol(const std::string& name) const;

private:
	Context& operator =(const Context&);

	SymbolTable* _globalSymTable;
	StackType _symTableStack;
	StackType _allSymbolTables;
	FunctionSymbol* _currentFunction;
	Symbol::DataType _currentDeclaredVarType;
};

} // namespace frontend

#endif // FRONTEND_CONTEXT_H
