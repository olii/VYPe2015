#include "ir/builder.h"
#include "ir/instruction.h"
#include "ir/value.h"

namespace ir {

Builder::Builder() : _functions(), _activeBasicBlock(nullptr)
{
}

Builder::~Builder()
{
}

Function* Builder::createFunction(const std::string& name, const std::vector<Value*>& arguments)
{
	if (getFunction(name) != nullptr)
		return nullptr;

	_functions[name] = new Function(name, arguments);
	return _functions[name];
}

Function* Builder::getFunction(const std::string& name) const
{
	std::map<std::string, Function*>::const_iterator itr = _functions.find(name);
	if (itr == _functions.end())
		return nullptr;

	return itr->second;
}

void Builder::setActiveFunction(ir::Function* function)
{
	_activeFunction = function;
}

BasicBlock* Builder::createBasicBlock()
{
	return new BasicBlock();
}

BasicBlock* Builder::getActiveBasicBlock() const
{
	return _activeBasicBlock;
}

void Builder::setActiveBasicBlock(BasicBlock* basicBlock)
{
	_activeBasicBlock = basicBlock;
}

void Builder::addBasicBlock(BasicBlock* basicBlock)
{
	if (basicBlock == nullptr)
		return;

	_activeFunction->addBasicBlock(basicBlock);
}

Value* Builder::createNamedValue(Value::DataType dataType, const std::string& name)
{
	return new NamedValue(dataType, name);
}

Value* Builder::createTemporaryValue(Value::DataType dataType)
{
	return new TemporaryValue(dataType);
}

Value* Builder::createConstantValue(int value)
{
	return new ConstantValue<int>(Value::DataType::INT, value);
}

Value* Builder::createConstantValue(char value)
{
	return new ConstantValue<char>(Value::DataType::CHAR, value);
}

Value* Builder::createConstantValue(const std::string& value)
{
	return new ConstantValue<std::string>(Value::DataType::STRING, value);
}

Value* Builder::createCall(const std::string& functionName, const std::vector<Value*> arguments)
{
	Function* function = getFunction(functionName);
	Value* retValue = createTemporaryValue(function->getReturnDataType());
	for (Value* arg : arguments)
	{
		if (arg->getType() == Value::Type::NAMED)
			_activeBasicBlock->addUse(arg);
	}
	_activeBasicBlock->addInstruction(new CallInstruction(retValue, function, arguments));
	return retValue;
}

Value* Builder::createDeclaration(const std::string& name, Value::DataType dataType)
{
	Value* value = createNamedValue(dataType, name);
	_activeBasicBlock->addDef(value);
	_activeBasicBlock->addInstruction(new DeclarationInstruction(value));
	return value;
}

template Value* Builder::createUnaryOperation<NotInstruction>(Value* operand);

template <typename T> Value* Builder::createUnaryOperation(Value* operand)
{
	static_assert(std::is_base_of<UnaryInstruction, T>::value, "Builder::createUnaryOperation template type needs to be derived from UnaryInstruction.");

	if (operand->getType() == Value::Type::NAMED)
		_activeBasicBlock->addUse(operand);

	Value* resultValue = createTemporaryValue(operand->getDataType());
	_activeBasicBlock->addInstruction(new T(resultValue, operand));
	return resultValue;
}

template Value* Builder::createBinaryOperation<AddInstruction>(Value* leftOperand, Value* rightOperand);
template Value* Builder::createBinaryOperation<SubtractInstruction>(Value* leftOperand, Value* rightOperand);
template Value* Builder::createBinaryOperation<MultiplyInstruction>(Value* leftOperand, Value* rightOperand);
template Value* Builder::createBinaryOperation<DivideInstruction>(Value* leftOperand, Value* rightOperand);
template Value* Builder::createBinaryOperation<ModuloInstruction>(Value* leftOperand, Value* rightOperand);
template Value* Builder::createBinaryOperation<LessInstruction>(Value* leftOperand, Value* rightOperand);
template Value* Builder::createBinaryOperation<LessEqualInstruction>(Value* leftOperand, Value* rightOperand);
template Value* Builder::createBinaryOperation<GreaterInstruction>(Value* leftOperand, Value* rightOperand);
template Value* Builder::createBinaryOperation<GreaterEqualInstruction>(Value* leftOperand, Value* rightOperand);
template Value* Builder::createBinaryOperation<EqualInstruction>(Value* leftOperand, Value* rightOperand);
template Value* Builder::createBinaryOperation<NotEqualInstruction>(Value* leftOperand, Value* rightOperand);
template Value* Builder::createBinaryOperation<AndInstruction>(Value* leftOperand, Value* rightOperand);
template Value* Builder::createBinaryOperation<OrInstruction>(Value* leftOperand, Value* rightOperand);

template <typename T> Value* Builder::createBinaryOperation(Value* leftOperand, Value* rightOperand)
{
	static_assert(std::is_base_of<BinaryInstruction, T>::value, "Builder::createBinaryOperation template type needs to be derived from BinaryInstruction.");

	if (leftOperand->getType() == Value::Type::NAMED)
		_activeBasicBlock->addUse(leftOperand);

	if (rightOperand->getType() == Value::Type::NAMED)
		_activeBasicBlock->addUse(rightOperand);

	Value* resultValue = createTemporaryValue(leftOperand->getDataType());
	_activeBasicBlock->addInstruction(new T(resultValue, leftOperand, rightOperand));
	return resultValue;
}

void Builder::createAssignment(Value* dest, Value* value)
{
	if (dest->getType() == Value::Type::NAMED)
		_activeBasicBlock->addDef(dest);

	if (value->getType() == Value::Type::NAMED)
		_activeBasicBlock->addUse(value);

	_activeBasicBlock->addInstruction(new AssignInstruction(dest, value));
}

void Builder::createJump(BasicBlock* destBlock)
{
	_activeBasicBlock->addSuccessor(destBlock);
	destBlock->addPredecessor(_activeBasicBlock);
	_activeBasicBlock->addInstruction(new JumpInstruction(destBlock));
}

void Builder::createConditionalJump(Value* condition, BasicBlock* ifTrue, BasicBlock* ifFalse)
{
	_activeBasicBlock->addSuccessor(ifTrue);
	_activeBasicBlock->addSuccessor(ifFalse);
	ifTrue->addPredecessor(_activeBasicBlock);
	ifFalse->addPredecessor(_activeBasicBlock);
	_activeBasicBlock->addInstruction(new CondJumpInstruction(condition, ifTrue, ifFalse));
}

void Builder::createReturn(Value* value)
{
	if (value != nullptr && value->getType() == Value::Type::NAMED)
		_activeBasicBlock->addUse(value);

	_activeBasicBlock->addInstruction(new ReturnInstruction(value));
}

std::string Builder::codeText() const
{
	std::stringstream codeStream;
	for (auto& keyValue : _functions)
		keyValue.second->text(codeStream);

	return codeStream.str();
}

}
