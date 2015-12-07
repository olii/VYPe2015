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

	void setExpectedReturnType(Symbol::DataType dataType);
	Symbol::DataType getExpectedReturnType() const;

	Symbol* findSymbol(const std::string& name);
	const Symbol* findSymbol(const std::string& name) const;

private:
	Context& operator =(const Context&);

	Symbol::DataType _expectedReturnType;
	SymbolTable* _globalSymTable;
	StackType _symTableStack;
	StackType _allSymbolTables;
};

} // namespace frontend

#endif // FRONTEND_CONTEXT_H
