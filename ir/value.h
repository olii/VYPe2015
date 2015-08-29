#ifndef IR_VALUE_H
#define IR_VALUE_H

#include <sstream>
#include <string>

namespace ir {

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

    Type getType() const;
    void setType(Type type);

    DataType getDataType() const;
    void setDataType(DataType dataType);

    virtual void text(std::stringstream& os) = 0;

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

    const std::string& getName() const;
    void setName(const std::string& name);

    virtual void text(std::stringstream& os) override;

private:
    std::string _name;
};

class TemporaryValue : public Value
{
public:
    TemporaryValue(Value::DataType dataType);
    virtual ~TemporaryValue();

    uint64_t getTemporaryId() const;
    void setTemporaryId(uint64_t temporaryId);
    std::string getSymbolicName() const;

    virtual void text(std::stringstream& os) override;

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

    ConstantType getConstantValue() const;
    void setConstantValue(const ConstantType& value);

    virtual void text(std::stringstream& os) override;

private:
    ConstantType _value;
};

} // namespace ir

#endif // IR_VALUE_H
