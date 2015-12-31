/**
 * Project VYPe 2015/2016
 * Marek Milkovič (xmilko01) & Oliver Nemček (xnemce03)
 **/

#include "frontend/symbol.h"

namespace frontend {

Symbol::Symbol(Type type, const std::string& name) : _type(type), _name(name)
{
}

Symbol::~Symbol()
{
}

std::string Symbol::dataTypeToString(Symbol::DataType dataType)
{
	switch (dataType)
	{
		case Symbol::DataType::INT:
			return "int";
		case Symbol::DataType::STRING:
			return "string";
		case Symbol::DataType::CHAR:
			return "char";
		default:
			break;
	}

	return "void";
}

Symbol::DataType Symbol::stringToDataType(const std::string& typeString)
{
	if (typeString == "int")
		return Symbol::DataType::INT;
	else if (typeString == "char")
		return Symbol::DataType::CHAR;
	else if (typeString == "string")
		return Symbol::DataType::STRING;

	return Symbol::DataType::VOID;
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
	: Symbol(Symbol::Type::VARIABLE, name), _dataType(dataType)
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
		: Symbol(Symbol::Type::FUNCTION, name), _returnType(returnType), _parameters(parameters), _defined(definition)
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

ManglingLinkSymbol::ManglingLinkSymbol(const std::string& name) : Symbol(Symbol::Type::MANGLING_LINK, name)
{
}

ManglingLinkSymbol::~ManglingLinkSymbol()
{
}

const ManglingLinkSymbol::FunctionList& ManglingLinkSymbol::getFunctions() const
{
	return _functions;
}

void ManglingLinkSymbol::addFunction(FunctionSymbol* functionSymbol)
{
	_functions.push_back(functionSymbol);
}

} // namespace frontend
