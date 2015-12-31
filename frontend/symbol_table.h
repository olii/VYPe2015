/**
 * Project VYPe 2015/2016
 * Marek Milkovič (xmilko01) & Oliver Nemček (xnemce03)
 **/

#ifndef FRONTEND_SYMBOL_TABLE_H
#define FRONTEND_SYMBOL_TABLE_H

#include "frontend/symbol.h"

namespace frontend {

class SymbolTable
{
public:
	using TableType = std::map<std::string, Symbol*>;

	SymbolTable();
	SymbolTable(const SymbolTable&);
	virtual ~SymbolTable();

	size_t getSize() const;

	const TableType& getAllSymbols() const;

	VariableSymbol* addVariable(VariableSymbol* variableSymbol);
	VariableSymbol* addVariable(const std::string& name, Symbol::DataType dataType);
	FunctionSymbol* addFunction(const std::string& name, Symbol::DataType returnType, const FunctionSymbol::ParameterList& parameters, bool definition);

	Symbol* findSymbol(const std::string& name);
	const Symbol* findSymbol(const std::string& name) const;

private:
	TableType _table;
};

} // namespace frontend

#endif // FRONTEND_SYMBOL_TABLE_H
