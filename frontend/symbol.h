#ifndef FRONTEND_SYMBOL_H
#define FRONTEND_SYMBOL_H

#include <map>
#include <vector>
#include <utility>
#include "ir/value.h"

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
	static ir::Value::DataType dataTypeToIrDataType(DataType dataType);

	Type getType() const;
	const std::string& getName() const;

	ir::Value* getIrValue() const;
	void setIrValue(ir::Value* value);

protected:
	Symbol(Type type, const std::string& name);
	Symbol& operator =(const Symbol&);

	void setName(const std::string& name);

	Type _type;
	std::string _name;
	ir::Value* _irValue;
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
		Parameter(const std::string& name_, Symbol::DataType dataType_) : name(name_), dataType(dataType_) {}
		Parameter(const Parameter& param) : name(param.name), dataType(param.dataType) {}
		Parameter(Parameter&& param) : name(std::move(param.name)), dataType(param.dataType) {}

		Parameter operator =(const Parameter& rhs)
		{
			name = rhs.name;
			dataType = rhs.dataType;
			return *this;
		}

		Parameter operator =(Parameter&& rhs)
		{
			name = std::move(rhs.name);
			dataType = rhs.dataType;
			return *this;
		}

		bool operator ==(const Parameter& rhs) const
		{
			return (name == rhs.name) && (dataType == rhs.dataType);
		}

		bool operator !=(const Parameter& rhs) const
		{
			return *this != rhs;
		}

		std::string name;
		Symbol::DataType dataType;
	};

	using ParameterList = std::vector<Parameter>;

	FunctionSymbol() = delete;
	FunctionSymbol(const std::string& name, DataType returnType, const ParameterList& parameters, bool definition);
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
