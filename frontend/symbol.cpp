#include "frontend/symbol.h"

namespace frontend {

Symbol::Symbol(Type type, const std::string& name, Symbol::DataType dataType) : _type(type), _name(name), _dataType(dataType)
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

Symbol::Type Symbol::getType() const
{
    return _type;
}

const std::string& Symbol::getName() const
{
    return _name;
}

Symbol::DataType Symbol::getDataType() const
{
    return _dataType;
}

FunctionSymbol::FunctionSymbol(const std::string& name, Symbol::DataType returnType, const ParameterList& parameters) :
        Symbol(Symbol::FUNCTION, name, returnType), _parameters(parameters), _defined(false)
{
}

FunctionSymbol::~FunctionSymbol()
{
}

Symbol::DataType FunctionSymbol::getReturnType() const
{
    return _dataType;
}

void FunctionSymbol::setReturnType(Symbol::DataType dataType)
{
    _dataType = dataType;
}

const FunctionSymbol::ParameterList& FunctionSymbol::getParameters() const
{
    return _parameters;
}

void FunctionSymbol::setParameters(const FunctionSymbol::ParameterList& parameters)
{
    _parameters = parameters;
}

bool FunctionSymbol::isFunctionDefined() const
{
    return _defined;
}

void FunctionSymbol::setFunctionDefined(bool set)
{
    _defined = set;
}

} // namespace frontend
