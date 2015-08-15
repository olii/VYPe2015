#ifndef FRONTEND_SYMBOL_H
#define FRONTEND_SYMBOL_H

#include <map>
#include <vector>

namespace frontend {

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

    Symbol() = delete;
    Symbol(const Symbol&) = delete;
    virtual ~Symbol();

    static DataType stringToDataType(const std::string& typeString);

    Type getType() const;
    const std::string& getName() const;

protected:
    Symbol(Type type, const std::string& name);
    Symbol& operator =(const Symbol&);

    void setName(const std::string& name);

    Type _type;
    std::string _name;
};

class VariableSymbol : public Symbol
{
public:
    VariableSymbol() = delete;
    VariableSymbol(const std::string& name, DataType dataType);
    VariableSymbol(const VariableSymbol&) = delete;
    virtual ~VariableSymbol();

    DataType getDataType() const;
    void setDataType(DataType dataType);

private:
    VariableSymbol& operator =(const VariableSymbol&);

    DataType _dataType;
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

    using ParameterList = std::vector<Parameter*>;

    FunctionSymbol() = delete;
    FunctionSymbol(const std::string& name, DataType returnType, const ParameterList& parameters);
    FunctionSymbol(const FunctionSymbol&) = delete;
    virtual ~FunctionSymbol();

    DataType getReturnType() const;
    void setReturnType(DataType dataType);

    const ParameterList& getParameters() const;
    void setParameters(const ParameterList& paramaters);

    bool isDefined() const;
    void setDefined(bool set);

private:
    FunctionSymbol& operator =(const FunctionSymbol&);

    DataType _returnType;
    ParameterList _parameters;
    bool _defined;

};

} // namespace frontend

#endif // FRONTEND_SYMBOL_H
