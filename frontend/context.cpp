#include "frontend/context.h"

namespace frontend {

Context::Context() : _globalSymTable(new SymbolTable), _symTableStack()
{
}

Context::~Context()
{
    for (StackType::iterator itr = _symTableStack.begin(); itr != _symTableStack.end(); )
    {
        delete *itr;
        itr = _symTableStack.erase(itr);
    }
}

void Context::newSymbolTable()
{
    _symTableStack.push_back(new SymbolTable());
}

void Context::popSymbolTable()
{
    if (_symTableStack.empty())
        return;

    delete _symTableStack.back();
    _symTableStack.pop_back();
}

SymbolTable* Context::globalSymbolTable()
{
    return _globalSymTable;
}

SymbolTable* Context::currentSymbolTable()
{
    if (_symTableStack.empty())
        return nullptr;

    return _symTableStack.back();
}

Symbol* Context::findSymbol(const std::string& name)
{
    Symbol* symbol = nullptr;

    // Reverse iterate over symbol table stack to find latest symbol with this name
    for (StackType::reverse_iterator itr = _symTableStack.rbegin(); itr != _symTableStack.rend(); ++itr)
    {
        SymbolTable* symbolTable = *itr;

        if ((symbol = symbolTable->findSymbol(name)) != nullptr)
            return symbol;
    }

    return _globalSymTable->findSymbol(name);
}

const Symbol* Context::findSymbol(const std::string& name) const
{
    const Symbol* symbol = nullptr;

    // Reverse iterate over symbol table stack to find latest symbol with this name
    for (StackType::const_reverse_iterator itr = _symTableStack.rbegin(); itr != _symTableStack.rend(); ++itr)
    {
        const SymbolTable* symbolTable = *itr;

        if ((symbol = symbolTable->findSymbol(name)) != nullptr)
            return symbol;
    }

    return _globalSymTable->findSymbol(name);
}

} // namespace frontend
