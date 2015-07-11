#include "frontend/symbol.h"

Symbol::Symbol()
{
}

Symbol::~Symbol()
{
}

DataType Symbol::stringToDataType(const std::string& typeString)
{
    if (typeString == "int")
        return INT;
    else if (typeString == "char")
        return CHAR;
    else if (typeString == "string")
        return STRING;

    return VOID;
}

const std::string& Symbol::getName() const
{
    return _name;
}

void Symbol::setName(const std::string& name)
{
    _name = name;
}

Type Symbol::getType() const
{
    return _type;
}

FunctionSymbol::FunctionSymbol()
{
}

FunctionSymbol::~FunctionSymbol()
{
}

DataType FunctionSymbol::getReturnType() const
{
    return _returnType;
}

void FunctionSymbol::setReturnType(DataType dataType)
{
    _returnType = dataType;
}

const SymbolTable* FunctionSymbol::getParameters() const
{
    return _parameters;
}

void FunctionSymbol::setParameters(SymbolTable* paramaters)
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
