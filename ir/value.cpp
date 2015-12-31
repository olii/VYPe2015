/**
 * Project VYPe 2015/2016
 * Marek Milkovič (xmilko01) & Oliver Nemček (xnemce03)
 **/

#include "ir/ir_visitor.h"
#include "ir/value.h"

namespace ir {

Value::Value(Value::Type type, Value::DataType dataType)
	: _type(type), _dataType(dataType)
{
}

Value::Value(const Value& value) : _type(value._type), _dataType(value._dataType)
{
}

Value::~Value()
{
}

std::string Value::dataTypeToString(DataType dataType)
{
	switch (dataType)
	{
		case DataType::INT:
			return "int";
		case DataType::CHAR:
			return "char";
		case DataType::STRING:
			return "string";
		case DataType::VOID:
			return "void";
	}

	return "";
}

Value::Type Value::getType() const
{
	return _type;
}

void Value::setType(Value::Type type)
{
	_type = type;
}

Value::DataType Value::getDataType() const
{
	return _dataType;
}

void Value::setDataType(Value::DataType dataType)
{
	_dataType = dataType;
}

NamedValue::NamedValue(Value::DataType dataType, const std::string& name) : Value(Value::Type::NAMED, dataType), _name(name)
{
}

NamedValue::~NamedValue()
{
}

void NamedValue::accept(IrVisitor& visitor)
{
	visitor.visit(this);
}

const std::string& NamedValue::getName() const
{
	return _name;
}

void NamedValue::setName(const std::string& name)
{
	_name = name;
}

uint64_t TemporaryValue::TemporaryIdPool = 0;

TemporaryValue::TemporaryValue(Value::DataType dataType) : Value(Value::Type::TEMPORARY, dataType), _temporaryId(TemporaryIdPool++)
{
}

TemporaryValue::~TemporaryValue()
{
}

void TemporaryValue::accept(IrVisitor& visitor)
{
	visitor.visit(this);
}

uint64_t TemporaryValue::getTemporaryId() const
{
	return _temporaryId;
}

void TemporaryValue::setTemporaryId(uint64_t temporaryId)
{
	_temporaryId = temporaryId;
}

std::string TemporaryValue::getSymbolicName() const
{
	std::stringstream ss;
	ss << "\%temp" << _temporaryId;
	return ss.str();
}

template <typename T> ConstantValue<T>::ConstantValue(Value::DataType dataType, const ConstantValue<T>::ConstantType& value)
	: Value(Value::Type::CONSTANT, dataType), _value(value)
{
}

template <typename T> ConstantValue<T>::~ConstantValue()
{
}

template <typename T> void ConstantValue<T>::accept(IrVisitor& visitor)
{
	visitor.visit(this);
}

template <typename T> typename ConstantValue<T>::ConstantType ConstantValue<T>::getConstantValue() const
{
	return _value;
}

template <typename T> void ConstantValue<T>::setConstantValue(const ConstantValue<T>::ConstantType& value)
{
	_value = value;
}

template class ConstantValue<int>;
template class ConstantValue<char>;
template class ConstantValue<std::string>;

} // namespace ir
