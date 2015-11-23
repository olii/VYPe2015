#ifndef IR_VALUE_H
#define IR_VALUE_H

#include <sstream>
#include <string>

namespace ir {

class IrVisitor;

class Value
{
public:
	enum class Type
	{
		NAMED,
		TEMPORARY,
		CONSTANT
	};

	enum class DataType
	{
		INT,
		CHAR,
		STRING,
		VOID
	};

	Value(const Value&);

	static std::string dataTypeToString(DataType dataType);

	virtual ~Value();

	virtual void accept(IrVisitor& visitor) = 0;

	Type getType() const;
	void setType(Type type);

	DataType getDataType() const;
	void setDataType(DataType dataType);

protected:
	Value(Type type, DataType dataType);

	Type _type;
	DataType _dataType;
};

class NamedValue : public Value
{
public:
	NamedValue(Value::DataType dataType, const std::string& name);
	virtual ~NamedValue();

	virtual void accept(IrVisitor& visitor) override;

	const std::string& getName() const;
	void setName(const std::string& name);

private:
	std::string _name;
};

class TemporaryValue : public Value
{
public:
	TemporaryValue(Value::DataType dataType);
	virtual ~TemporaryValue();

	virtual void accept(IrVisitor& visitor) override;

	uint64_t getTemporaryId() const;
	void setTemporaryId(uint64_t temporaryId);
	std::string getSymbolicName() const;

	static uint64_t TemporaryIdPool;
private:
	uint64_t _temporaryId;
};

template <typename T> class ConstantValue : public Value
{
public:
	using ConstantType = T;

	ConstantValue(Value::DataType dataType, const ConstantType& value);
	virtual ~ConstantValue();

	virtual void accept(IrVisitor& visitor) override;

	ConstantType getConstantValue() const;
	void setConstantValue(const ConstantType& value);

private:
	ConstantType _value;
};

} // namespace ir

#endif // IR_VALUE_H
