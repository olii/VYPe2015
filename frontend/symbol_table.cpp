#include "frontend/symbol_table.h"

namespace frontend {

SymbolTable::SymbolTable() : _table()
{
}

SymbolTable::~SymbolTable()
{
	for (auto& pair : _table)
		delete pair.second;
}

size_t SymbolTable::getSize() const
{
	return _table.size();
}

const SymbolTable::TableType& SymbolTable::getAllSymbols() const
{
	return _table;
}

VariableSymbol* SymbolTable::addVariable(VariableSymbol* variableSymbol)
{
	// Check whether symbol with this name already exists in the current table
	if (findSymbol(variableSymbol->getName()))
		return nullptr;

	_table[variableSymbol->getName()] = variableSymbol;
	return variableSymbol;
}

VariableSymbol* SymbolTable::addVariable(const std::string& name, Symbol::DataType dataType)
{
	// Check whether symbol with this name already exists in the current table
	if (findSymbol(name))
		return nullptr;

	return addVariable(new VariableSymbol(name, dataType));
}

FunctionSymbol* SymbolTable::addFunction(const std::string& name, Symbol::DataType returnType, const FunctionSymbol::ParameterList& parameters, bool definition)
{
	// Check whether symbol with this name already exists in the current table
	Symbol* symbol = findSymbol(name);

	// If it is declaration of function
	if (!definition)
	{
		// If there was such symbol it is error, we cannot declare the same symbol twice
		if (symbol != nullptr)
			return nullptr;
	}
	// Definition
	else
	{
		// If there is such symbol
		if (symbol != nullptr)
		{
			// If it is not function symbol, then it must be variable, therefore error
			FunctionSymbol* func = dynamic_cast<FunctionSymbol*>(symbol);
			if (func == nullptr)
				return nullptr;

			// If the function was already defined, error
			if (func->isDefined())
				return nullptr;

			// Check whether return types match
			if (func->getReturnType() != returnType)
				return nullptr;

			// Check whether number of parameters match
			if (func->getParameters().size() != parameters.size())
				return nullptr;

			// Check whether type of every parameter match
			const FunctionSymbol::ParameterList& declParams = func->getParameters();
			for (size_t i = 0; i < parameters.size(); ++i)
			{
				if (declParams[i]->getDataType() != parameters[i]->getDataType())
					return nullptr;
			}

			for (const auto& param : declParams)
				delete param;

			// Rewrite placeholder declaration parameters with the actual definition parameters and set the function as defined
			func->setParameters(parameters);
			func->setDefined(true);
			return func;
		}
	}

	_table[name] = new FunctionSymbol(name, returnType, parameters, definition);
	return static_cast<FunctionSymbol*>(_table[name]);
}

Symbol* SymbolTable::findSymbol(const std::string& name)
{
	TableType::iterator itr = _table.find(name);
	if (itr == _table.end())
		return nullptr;

	return itr->second;
}

const Symbol* SymbolTable::findSymbol(const std::string& name) const
{
	TableType::const_iterator itr = _table.find(name);
	if (itr == _table.end())
		return nullptr;

	return itr->second;
}

} // namespace frontend
