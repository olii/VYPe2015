#include "ir/function.h"
#include "ir/ir_visitor.h"

namespace ir {

Function::Function(const std::string& name, const std::vector<Value*>& parameters)
	: _name(name), _basicBlocks(), _parameters(parameters), _returnDataType(ir::Value::DataType::VOID)
{
}

Function::~Function()
{
}

void Function::accept(IrVisitor& visitor)
{
	visitor.visit(this);
}

BasicBlock* Function::getEntryBasicBlock() const
{
	return _basicBlocks.front();
}

std::vector<BasicBlock*>& Function::getBasicBlocks()
{
	return _basicBlocks;
}

void Function::addBasicBlock(BasicBlock* basicBlock)
{
	_basicBlocks.push_back(basicBlock);
}

const std::string& Function::getName() const
{
	return _name;
}

const std::vector<Value*>& Function::getParameters() const
{
	return _parameters;
}

Value::DataType Function::getReturnDataType() const
{
	return _returnDataType;
}

} // namespace ir
