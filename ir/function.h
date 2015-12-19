#ifndef IR_FUNCTION_H
#define IR_FUNCTION_H

#include "ir/basic_block.h"
#include "ir/value.h"

namespace ir {

class IrVisitor;

class Function
{
public:
	Function(const std::string& name, Value::DataType returnDataType, const std::vector<Value*>& _parameters);
	~Function();

	void accept(IrVisitor& visitor);

	BasicBlock* getEntryBasicBlock() const;
	BasicBlock* getTerminalBasicBlock() const;
	std::vector<BasicBlock*>& getBasicBlocks();
	void addBasicBlock(BasicBlock* basicBlock);

	const std::string& getName() const;
	const std::vector<Value*>& getParameters() const;
	Value::DataType getReturnDataType() const;

private:
	std::string _name;
	Value::DataType _returnDataType;
	std::vector<Value*> _parameters;
	std::vector<BasicBlock*> _basicBlocks;
};

} // namespace ir

#endif // IR_FUNCTION_H
