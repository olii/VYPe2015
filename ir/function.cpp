#include "ir/function.h"

namespace ir {

Function::Function(const std::string& name, const std::vector<Value*>& parameters)
    : _name(name), _basicBlocks(), _parameters(parameters), _returnDataType(ir::Value::DataType::VOID)
{
}

Function::~Function()
{
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

void Function::text(std::stringstream& os)
{
    os << "function " << getName() << "(";
    for (Value* param : _parameters)
    {
        param->text(os);
        os << ":" << Value::dataTypeToString(param->getDataType()) << ", ";
    }
    os << "):" << Value::dataTypeToString(_returnDataType) << "\n";

    for (BasicBlock* bb : _basicBlocks)
    {
        bb->text(os);
        os << ":\n";

        for (Instruction* ins : bb->getInstructions())
            ins->text(os);
    }

    os << "\n";
}

} // namespace ir