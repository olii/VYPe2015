#include "ir/function.h"
#include "ir/ir_visitor.h"

namespace ir {

Function::Function(const std::string& name, const std::vector<Value*>& parameters)
	: _name(name), _basicBlocks(), _parameters(parameters), _returnDataType(ir::Value::DataType::VOID)
{
}

Function::~Function()
{
	for (auto& bb : _basicBlocks)
		delete bb;
}

void Function::accept(IrVisitor& visitor)
{
	visitor.visit(this);
}

BasicBlock* Function::getEntryBasicBlock() const
{
	return _basicBlocks.front();
}

BasicBlock* Function::getTerminalBasicBlock() const
{
	return _basicBlocks.back();
}

std::vector<BasicBlock*>& Function::getBasicBlocks()
{
	return _basicBlocks;
}

void Function::addBasicBlock(BasicBlock* basicBlock)
{
	// So far no entry and terminal block, just insert regularry
	if (_basicBlocks.size() < 2)
		_basicBlocks.push_back(basicBlock);
	else
		_basicBlocks.insert(_basicBlocks.end() - 1, basicBlock);
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
