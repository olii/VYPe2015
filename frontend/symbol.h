#ifndef FRONTEND_SYMBOL_H
#define FRONTEND_SYMBOL_H

#include <map>

class SymbolTable;

class Symbol
{
public:
    enum Type
    {
        VARIABLE    = 0,
        FUNCTION
    };

    enum DataType
    {
        VOID        = 0,
        INT,
        CHAR,
        STRING
    };

    Symbol();
    Symbol(const Symbol&) = delete;
    virtual ~Symbol();

    static DataType stringToDataType(const std::string& typeString);

    const std::string& getName() const;

    Type getType() const;

private:
    Symbol& operator =(const Symbol&);

    void setName(const std::string& name);

    std::string _name;
    Type _type;
};

class FunctionSymbol : public Symbol
{
public:
    struct Parameter
    {
        Parameter(Symbol::DataType dataType_, const std::string& name_) : dataType(dataType_), name(name_) {}

        Symbol::DataType dataType;
        std::string name;
    };

    typedef std::vector<Parameter*> ParameterList;

    FunctionSymbol();
    FunctionSymbol(const FunctionSymbol&) = delete;
    virtual ~FunctionSymbol();

    DataType getReturnType() const;
    void setReturnType(DataType dataType);

    const ParameterList& getParameters() const;
    void setParameters(const ParameterList& paramaters);

    bool isFunctionDefined() const;
    void setFunctionDefined(bool set);

private:
    FunctionSymbol& operator =(const FunctionSymbol&);

    DataType _returnType;
    ParameterList _parameters;
    bool _defined;

};

#endif
