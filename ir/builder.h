/**
 * Project VYPe 2015/2016
 * Marek Milkovič (xmilko01) & Oliver Nemček (xnemce03)
 **/

#ifndef IR_BUILDER_H
#define IR_BUILDER_H

#include <vector>
#include <map>
#include "ir/basic_block.h"
#include "ir/function.h"

namespace ir {

class Builder
{
public:
	Builder();
	virtual ~Builder();

	Function* createFunction(const std::string& name, Value::DataType returnDataType, const std::vector<Value*>& parameters);
	Function* getFunction(const std::string& name) const;
	Function* getActiveFunction() const;
	void setActiveFunction(ir::Function* function);

	BasicBlock* createBasicBlock();
	BasicBlock* getActiveBasicBlock() const;
	void setActiveBasicBlock(BasicBlock* basicBlock);
	void addBasicBlock(BasicBlock* basicBlock);

	Value* createNamedValue(Value::DataType dataType, const std::string& name);
	Value* createTemporaryValue(Value::DataType dataType);
	Value* createConstantValue(int value);
	Value* createConstantValue(char value);
	Value* createConstantValue(const std::string& value);
	Value* createCall(const std::string& functionName, const std::vector<Value*> arguments, bool createValueForResult);
	Value* createBuiltinCall(const std::string& functionName, ir::Value::DataType returnDataType, const std::vector<Value*> arguments, bool createValueForResult);
	Value* createDeclaration(const std::string& name, Value::DataType dataType);
	template <typename T> Value* createUnaryOperation(Value* operand, Value::DataType resultDataType);
	template <typename T> Value* createBinaryOperation(Value* leftOperand, Value* rightOperand, Value::DataType resultDataType);

	void createAssignment(Value* dest, Value* value);
	void createJump(BasicBlock* destBlock);
	void createConditionalJump(Value* condition, BasicBlock* ifTrue, BasicBlock* ifFalse);
	void createReturn(Value* value);

	std::string codeText() const;

    const std::map<std::string, Function *> &getFunctions() const;

private:
	std::map<std::string, Function*> _functions;
	Function* _activeFunction;
	BasicBlock* _activeBasicBlock;
	std::vector<Value*> _managedValues;
};

} // namespace ir

#endif // IR_BUILDER_H
