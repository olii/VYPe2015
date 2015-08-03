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

    typedef std::vector<SymbolTable*> StackType;

    void newSymbolTable();
    void popSymbolTable();

    SymbolTable* globalSymbolTable();
    SymbolTable* currentSymbolTable();

    Symbol* findSymbol(const std::string& name);
    const Symbol* findSymbol(const std::string& name) const;

private:
    Context& operator =(const Context&);

    StackType _symbolStack;
};

} // namespace frontend

#endif // FRONTEND_CONTEXT_H
