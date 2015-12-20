#include "frontend/context.h"

namespace frontend {

Context::Context() : _globalSymTable(new SymbolTable), _symTableStack(), _allSymbolTables(),
	_currentFunction(nullptr), _currentDeclaredVarType(Symbol::DataType::VOID)
{
}

Context::~Context()
{
	delete _globalSymTable;
	for (auto& symTable : _allSymbolTables)
		delete symTable;
}

void Context::newSymbolTable()
{
	_symTableStack.push_back(new SymbolTable());
	_allSymbolTables.push_back(_symTableStack.back());
}

void Context::popSymbolTable()
{
	if (_symTableStack.empty())
		return;

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

void Context::setCurrentFunction(FunctionSymbol* currentFunction)
{
	_currentFunction = currentFunction;
}

FunctionSymbol* Context::getCurrentFunction() const
{
	return _currentFunction;
}

Symbol::DataType Context::getCurrentDeclaredVarType() const
{
	return _currentDeclaredVarType;
}

void Context::setCurrentDeclaredVarType(Symbol::DataType dataType)
{
	_currentDeclaredVarType = dataType;
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
