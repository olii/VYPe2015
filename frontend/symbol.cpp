#include "frontend/symbol.h"

namespace frontend {

Symbol::Symbol(Type type, const std::string& name) : _type(type), _name(name)
{
}

Symbol::~Symbol()
{
}

Symbol::DataType Symbol::stringToDataType(const std::string& typeString)
{
	if (typeString == "int")
		return INT;
	else if (typeString == "char")
		return CHAR;
	else if (typeString == "string")
		return STRING;

	return VOID;
}

ir::Value::DataType Symbol::dataTypeToIrDataType(Symbol::DataType dataType)
{
	switch (dataType)
	{
		case Symbol::DataType::INT:
			return ir::Value::DataType::INT;
		case Symbol::DataType::CHAR:
			return ir::Value::DataType::CHAR;
		case Symbol::DataType::STRING:
			return ir::Value::DataType::STRING;
		case Symbol::DataType::VOID:
			return ir::Value::DataType::VOID;
	}

	return ir::Value::DataType::VOID;
}

Symbol::Type Symbol::getType() const
{
	return _type;
}

const std::string& Symbol::getName() const
{
	return _name;
}

ir::Value* Symbol::getIrValue() const
{
	return _irValue;
}

void Symbol::setIrValue(ir::Value* irValue)
{
	_irValue = irValue;
}

VariableSymbol::VariableSymbol(const std::string& name, Symbol::DataType dataType)
	: Symbol(Symbol::VARIABLE, name), _dataType(dataType)
{
}

VariableSymbol::~VariableSymbol()
{
}

Symbol::DataType VariableSymbol::getDataType() const
{
	return _dataType;
}

void VariableSymbol::setDataType(Symbol::DataType dataType)
{
	_dataType = dataType;
}

FunctionSymbol::FunctionSymbol(const std::string& name, Symbol::DataType returnType, const ParameterList& parameters, bool definition)
		: Symbol(Symbol::FUNCTION, name), _returnType(returnType), _parameters(parameters), _defined(definition)
{
}

FunctionSymbol::~FunctionSymbol()
{
}

Symbol::DataType FunctionSymbol::getReturnType() const
{
	return _returnType;
}

void FunctionSymbol::setReturnType(Symbol::DataType returnType)
{
	_returnType = returnType;
}

const FunctionSymbol::ParameterList& FunctionSymbol::getParameters() const
{
	return _parameters;
}

void FunctionSymbol::setParameters(const FunctionSymbol::ParameterList& parameters)
{
	_parameters = parameters;
}

bool FunctionSymbol::isDefined() const
{
	return _defined;
}

void FunctionSymbol::setDefined(bool set)
{
	_defined = set;
}

} // namespace frontend
