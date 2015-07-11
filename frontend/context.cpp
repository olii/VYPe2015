#include "frontend/context.h"

Context::Context() : _symbolStack()
{
}

Context::~Context()
{
    for (StackType::iterator itr = _symbolStack.begin(); itr != _symbolStack.end(); )
    {
        delete *itr;
        itr = _symbolStack.erase(itr);
    }
}

void Context::newSymbolTable()
{
    _symbolStack.push_back(new SymbolTable());
}

void Context::popSymbolTable()
{
    delete _symbolStack.back();
    _symbolStack.pop_back();
}

SymbolTable* Context::globalSymbolTable()
{
    return _symbolStack.front();
}

SymbolTable* Context::currentSymbolTable()
{
    return _symbolStack.back();
}

Symbol* Context::findSymbol(const std::string& name)
{
    Symbol* symbol = nullptr;

    // Reverse iterate over symbol table stack to find latest symbol with this name
    for (StackType::reverse_iterator itr = _symbolStack.rbegin(); itr != _symbolStack.rend(); ++itr)
    {
        SymbolTable* symbolTable = *itr;

        if ((symbol = symbolTable->findSymbol(name)) != nullptr)
            return symbol;
    }

    return nullptr;
}

const Symbol* Context::findSymbol(const std::string& name) const
{
    const Symbol* symbol = nullptr;

    // Reverse iterate over symbol table stack to find latest symbol with this name
    for (StackType::const_reverse_iterator itr = _symbolStack.rbegin(); itr != _symbolStack.rend(); ++itr)
    {
        const SymbolTable* symbolTable = *itr;

        if ((symbol = symbolTable->findSymbol(name)) != nullptr)
            return symbol;
    }

    return nullptr;
}
