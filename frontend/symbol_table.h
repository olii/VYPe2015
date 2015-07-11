#ifndef FRONTEND_SYMBOL_TABLE_H
#define FRONTEND_SYMBOL_TABLE_H

#include "frontend/symbol.h"

class SymbolTable
{
public:
    typedef std::map<std::string, Symbol*> TableType;

    SymbolTable();
    SymbolTable(const SymbolTable&);
    virtual ~SymbolTable();

    size_t getSize() const;

    Symbol* addVariable(const std::string& name, Symbol::DataType dataType);
    FunctionSymbol* addFunction(const std::string& name, Symbol::DataType returnType, const FunctionSymbol::ParameterList& parameters, bool definition);

    Symbol* findSymbol(const std::string& name);
    const Symbol* findSymbol(const std::string& name) const;

private:
    TableType _table;
};

#endif
