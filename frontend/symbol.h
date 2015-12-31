/**
 * Project VYPe 2015/2016
 * Marek Milkovič (xmilko01) & Oliver Nemček (xnemce03)
 **/

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
	enum class Type
	{
		VARIABLE,
		FUNCTION,
		MANGLING_LINK
	};

	enum class DataType
	{
		VOID,
		INT,
		CHAR,
		STRING
	};

	Symbol() = delete;
	Symbol(const Symbol&) = delete;
	virtual ~Symbol();

	static std::string dataTypeToString(DataType dataType);
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
	using ParameterList = std::vector<VariableSymbol*>;

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

class ManglingLinkSymbol : public Symbol
{
public:
	using FunctionList = std::vector<FunctionSymbol*>;

	ManglingLinkSymbol() = delete;
	ManglingLinkSymbol(const std::string& name);
	ManglingLinkSymbol(const ManglingLinkSymbol&) = delete;
	virtual ~ManglingLinkSymbol();

	const std::vector<FunctionSymbol*>& getFunctions() const;
	void addFunction(FunctionSymbol* functionSymbol);

private:
	FunctionList _functions;
};

} // namespace frontend

#endif // FRONTEND_SYMBOL_H
