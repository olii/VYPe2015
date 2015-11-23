#ifndef IR_FUNCTION_H
#define IR_FUNCTION_H

#include "ir/basic_block.h"
#include "ir/value.h"

namespace ir {

class Function
{
public:
	Function(const std::string& name, const std::vector<Value*>& _parameters);
	~Function();

	BasicBlock* getEntryBasicBlock() const;
	std::vector<BasicBlock*>& getBasicBlocks();
	void addBasicBlock(BasicBlock* basicBlock);

	const std::string& getName() const;
	const std::vector<Value*>& getParameters() const;
	Value::DataType getReturnDataType() const;

	void text(std::stringstream& os);

private:
	std::string _name;
	std::vector<BasicBlock*> _basicBlocks;
	std::vector<Value*> _parameters;
	Value::DataType _returnDataType;
};

} // namespace ir

#endif // IR_FUNCTION_H
