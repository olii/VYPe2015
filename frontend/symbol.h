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
    Symbol(Type type, const std::string& name, DataType dataType);
    Symbol(const Symbol&) = delete;
    virtual ~Symbol();

    static DataType stringToDataType(const std::string& typeString);

    Type getType() const;
    const std::string& getName() const;
    DataType getDataType() const;

protected:
    Symbol& operator =(const Symbol&);

    void setName(const std::string& name);

    Type _type;
    std::string _name;
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

    typedef std::vector<Parameter*> ParameterList;

    FunctionSymbol() = delete;
    FunctionSymbol(const std::string& name, DataType returnType, const ParameterList& parameters);
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

    ParameterList _parameters;
    bool _defined;

};

} // namespace frontend

#endif // FRONTEND_SYMBOL_H
