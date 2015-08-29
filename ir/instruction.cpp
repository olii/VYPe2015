#include "ir/basic_block.h"
#include "ir/function.h"
#include "ir/instruction.h"
#include "ir/value.h"

namespace ir {

Instruction::Instruction()
{
}

Instruction::~Instruction()
{
}

UnaryInstruction::UnaryInstruction(Value* operand) : _operand(operand)
{
}

UnaryInstruction::~UnaryInstruction()
{
}

Value* UnaryInstruction::getOperand()
{
    return _operand;
}

BinaryInstruction::BinaryInstruction(Value* leftOperand, Value* rightOperand) : _leftOperand(leftOperand), _rightOperand(rightOperand)
{
}

BinaryInstruction::~BinaryInstruction()
{
}

Value* BinaryInstruction::getLeftOperand()
{
    return _leftOperand;
}

Value* BinaryInstruction::getRightOperand()
{
    return _rightOperand;
}

ResultInstruction::ResultInstruction(Value* result) : _result(result)
{
}

ResultInstruction::~ResultInstruction()
{
}

Value* ResultInstruction::getResult()
{
    return _result;
}

AssignInstruction::AssignInstruction(Value* dest, Value* value) : ResultInstruction(dest), UnaryInstruction(value)
{
}

AssignInstruction::~AssignInstruction()
{
}

void AssignInstruction::text(std::stringstream& os)
{
    getResult()->text(os);
    os << " = ";
    getOperand()->text(os);
    os << "\n";
}

DeclarationInstruction::DeclarationInstruction(Value* declValue) : UnaryInstruction(declValue)
{
}

DeclarationInstruction::~DeclarationInstruction()
{
}

void DeclarationInstruction::text(std::stringstream& os)
{
    os << "declare ";
    getOperand()->text(os);
    os << ":" << Value::dataTypeToString(getOperand()->getDataType()) << "\n";
}

JumpInstruction::JumpInstruction(BasicBlock* followingBasicBlock) : _followingBasicBlock(followingBasicBlock)
{
}

JumpInstruction::~JumpInstruction()
{
}

BasicBlock* JumpInstruction::getFollowingBasicBlock()
{
    return _followingBasicBlock;
}

void JumpInstruction::text(std::stringstream& os)
{
    os << "jump ";
    getFollowingBasicBlock()->text(os);
    os << "\n";
}

CondJumpInstruction::CondJumpInstruction(Value* condition, BasicBlock* trueBasicBlock, BasicBlock* falseBasicBlock)
    : _condition(condition), _trueBasicBlock(trueBasicBlock), _falseBasicBlock(falseBasicBlock)
{
}

CondJumpInstruction::~CondJumpInstruction()
{
}

Value* CondJumpInstruction::getCondition()
{
    return _condition;
}

BasicBlock* CondJumpInstruction::getTrueBasicBlock()
{
    return _trueBasicBlock;
}

BasicBlock* CondJumpInstruction::getFalseBasicBlock()
{
    return _falseBasicBlock;
}

void CondJumpInstruction::text(std::stringstream& os)
{
    os << "jumpif ";
    getCondition()->text(os);
    os << ", ";
    getTrueBasicBlock()->text(os);
    os << ", ";
    getFalseBasicBlock()->text(os);
    os << "\n";
}

ReturnInstruction::ReturnInstruction(Value* returnValue) : UnaryInstruction(returnValue)
{
}

ReturnInstruction::~ReturnInstruction()
{
}

void ReturnInstruction::text(std::stringstream& os)
{
    os << "return ";
    getOperand()->text(os);
    os << "\n";
}

CallInstruction::CallInstruction(Value* returnValue, Function* function, const std::vector<Value*>& arguments)
    : ResultInstruction(returnValue), _function(function), _arguments(arguments)
{
}

CallInstruction::~CallInstruction()
{
}

Function* CallInstruction::getFunction()
{
    return _function;
}

std::vector<Value*>& CallInstruction::getArguments()
{
    return _arguments;
}

void CallInstruction::text(std::stringstream& os)
{
    getResult()->text(os);
    os << " = call " << getFunction()->getName();
    for (Value* arg : getArguments())
    {
        os << ", ";
        arg->text(os);
    }
    os << "\n";
}

AddInstruction::AddInstruction(Value* result, Value* leftOperand, Value* rightOperand) : ResultInstruction(result), BinaryInstruction(leftOperand, rightOperand)
{
}

AddInstruction::~AddInstruction()
{
}

void AddInstruction::text(std::stringstream& os)
{
    getResult()->text(os);
    os << " = add ";
    getLeftOperand()->text(os);
    os << ", ";
    getRightOperand()->text(os);
    os << "\n";
}

SubtractInstruction::SubtractInstruction(Value* result, Value* leftOperand, Value* rightOperand) : ResultInstruction(result), BinaryInstruction(leftOperand, rightOperand)
{
}

SubtractInstruction::~SubtractInstruction()
{
}

void SubtractInstruction::text(std::stringstream& os)
{
    getResult()->text(os);
    os << " = subtract ";
    getLeftOperand()->text(os);
    os << ", ";
    getRightOperand()->text(os);
    os << "\n";
}

MultiplyInstruction::MultiplyInstruction(Value* result, Value* leftOperand, Value* rightOperand) : ResultInstruction(result), BinaryInstruction(leftOperand, rightOperand)
{
}

MultiplyInstruction::~MultiplyInstruction()
{
}

void MultiplyInstruction::text(std::stringstream& os)
{
    getResult()->text(os);
    os << " = multiply ";
    getLeftOperand()->text(os);
    os << ", ";
    getRightOperand()->text(os);
    os << "\n";
}

DivideInstruction::DivideInstruction(Value* result, Value* leftOperand, Value* rightOperand) : ResultInstruction(result), BinaryInstruction(leftOperand, rightOperand)
{
}

DivideInstruction::~DivideInstruction()
{
}

void DivideInstruction::text(std::stringstream& os)
{
    getResult()->text(os);
    os << " = divide ";
    getLeftOperand()->text(os);
    os << ", ";
    getRightOperand()->text(os);
    os << "\n";
}

ModuloInstruction::ModuloInstruction(Value* result, Value* leftOperand, Value* rightOperand) : ResultInstruction(result), BinaryInstruction(leftOperand, rightOperand)
{
}

ModuloInstruction::~ModuloInstruction()
{
}

void ModuloInstruction::text(std::stringstream& os)
{
    getResult()->text(os);
    os << " = modulo ";
    getLeftOperand()->text(os);
    os << ", ";
    getRightOperand()->text(os);
    os << "\n";
}

LessInstruction::LessInstruction(Value* result, Value* leftOperand, Value* rightOperand) : ResultInstruction(result), BinaryInstruction(leftOperand, rightOperand)
{
}

LessInstruction::~LessInstruction()
{
}

void LessInstruction::text(std::stringstream& os)
{
    getResult()->text(os);
    os << " = less ";
    getLeftOperand()->text(os);
    os << ", ";
    getRightOperand()->text(os);
    os << "\n";
}

LessEqualInstruction::LessEqualInstruction(Value* result, Value* leftOperand, Value* rightOperand) : ResultInstruction(result), BinaryInstruction(leftOperand, rightOperand)
{
}

LessEqualInstruction::~LessEqualInstruction()
{
}

void LessEqualInstruction::text(std::stringstream& os)
{
    getResult()->text(os);
    os << " = lesseq ";
    getLeftOperand()->text(os);
    os << ", ";
    getRightOperand()->text(os);
    os << "\n";
}

GreaterInstruction::GreaterInstruction(Value* result, Value* leftOperand, Value* rightOperand) : ResultInstruction(result), BinaryInstruction(leftOperand, rightOperand)
{
}

GreaterInstruction::~GreaterInstruction()
{
}

void GreaterInstruction::text(std::stringstream& os)
{
    getResult()->text(os);
    os << " = greater ";
    getLeftOperand()->text(os);
    os << ", ";
    getRightOperand()->text(os);
    os << "\n";
}

GreaterEqualInstruction::GreaterEqualInstruction(Value* result, Value* leftOperand, Value* rightOperand) : ResultInstruction(result), BinaryInstruction(leftOperand, rightOperand)
{
}

GreaterEqualInstruction::~GreaterEqualInstruction()
{
}

void GreaterEqualInstruction::text(std::stringstream& os)
{
    getResult()->text(os);
    os << " = greatereq ";
    getLeftOperand()->text(os);
    os << ", ";
    getRightOperand()->text(os);
    os << "\n";
}

EqualInstruction::EqualInstruction(Value* result, Value* leftOperand, Value* rightOperand) : ResultInstruction(result), BinaryInstruction(leftOperand, rightOperand)
{
}

EqualInstruction::~EqualInstruction()
{
}

void EqualInstruction::text(std::stringstream& os)
{
    getResult()->text(os);
    os << " = equal ";
    getLeftOperand()->text(os);
    os << ", ";
    getRightOperand()->text(os);
    os << "\n";
}

NotEqualInstruction::NotEqualInstruction(Value* result, Value* leftOperand, Value* rightOperand) : ResultInstruction(result), BinaryInstruction(leftOperand, rightOperand)
{
}

NotEqualInstruction::~NotEqualInstruction()
{
}

void NotEqualInstruction::text(std::stringstream& os)
{
    getResult()->text(os);
    os << " = notequal ";
    getLeftOperand()->text(os);
    os << ", ";
    getRightOperand()->text(os);
    os << "\n";
}

AndInstruction::AndInstruction(Value* result, Value* leftOperand, Value* rightOperand) : ResultInstruction(result), BinaryInstruction(leftOperand, rightOperand)
{
}

AndInstruction::~AndInstruction()
{
}

void AndInstruction::text(std::stringstream& os)
{
    getResult()->text(os);
    os << " = and ";
    getLeftOperand()->text(os);
    os << ", ";
    getRightOperand()->text(os);
    os << "\n";
}

OrInstruction::OrInstruction(Value* result, Value* leftOperand, Value* rightOperand) : ResultInstruction(result), BinaryInstruction(leftOperand, rightOperand)
{
}

OrInstruction::~OrInstruction()
{
}

void OrInstruction::text(std::stringstream& os)
{
    getResult()->text(os);
    os << " = or ";
    getLeftOperand()->text(os);
    os << ", ";
    getRightOperand()->text(os);
    os << "\n";
}

NotInstruction::NotInstruction(Value* result, Value* operand) : ResultInstruction(result), UnaryInstruction(operand)
{
}

NotInstruction::~NotInstruction()
{
}

void NotInstruction::text(std::stringstream& os)
{
    getResult()->text(os);
    os << " = not ";
    getOperand()->text(os);
    os << "\n";
}

} // namespace ir
