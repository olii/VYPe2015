#include "ir/basic_block.h"
#include "ir/function.h"
#include "ir/instruction.h"
#include "ir/ir_visitor.h"
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

void AssignInstruction::accept(IrVisitor& visitor)
{
	visitor.visit(this);
}

DeclarationInstruction::DeclarationInstruction(Value* declValue) : UnaryInstruction(declValue)
{
}

DeclarationInstruction::~DeclarationInstruction()
{
}

void DeclarationInstruction::accept(IrVisitor& visitor)
{
	visitor.visit(this);
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

void JumpInstruction::accept(IrVisitor& visitor)
{
	visitor.visit(this);
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

void CondJumpInstruction::accept(IrVisitor& visitor)
{
	visitor.visit(this);
}

ReturnInstruction::ReturnInstruction(Value* returnValue) : UnaryInstruction(returnValue)
{
}

ReturnInstruction::~ReturnInstruction()
{
}

void ReturnInstruction::accept(IrVisitor& visitor)
{
	visitor.visit(this);
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

void CallInstruction::accept(IrVisitor& visitor)
{
	visitor.visit(this);
}

AddInstruction::AddInstruction(Value* result, Value* leftOperand, Value* rightOperand) : ResultInstruction(result), BinaryInstruction(leftOperand, rightOperand)
{
}

AddInstruction::~AddInstruction()
{
}

void AddInstruction::accept(IrVisitor& visitor)
{
	visitor.visit(this);
}

SubtractInstruction::SubtractInstruction(Value* result, Value* leftOperand, Value* rightOperand) : ResultInstruction(result), BinaryInstruction(leftOperand, rightOperand)
{
}

SubtractInstruction::~SubtractInstruction()
{
}

void SubtractInstruction::accept(IrVisitor& visitor)
{
	visitor.visit(this);
}

MultiplyInstruction::MultiplyInstruction(Value* result, Value* leftOperand, Value* rightOperand) : ResultInstruction(result), BinaryInstruction(leftOperand, rightOperand)
{
}

MultiplyInstruction::~MultiplyInstruction()
{
}

void MultiplyInstruction::accept(IrVisitor& visitor)
{
	visitor.visit(this);
}

DivideInstruction::DivideInstruction(Value* result, Value* leftOperand, Value* rightOperand) : ResultInstruction(result), BinaryInstruction(leftOperand, rightOperand)
{
}

DivideInstruction::~DivideInstruction()
{
}

void DivideInstruction::accept(IrVisitor& visitor)
{
	visitor.visit(this);
}

ModuloInstruction::ModuloInstruction(Value* result, Value* leftOperand, Value* rightOperand) : ResultInstruction(result), BinaryInstruction(leftOperand, rightOperand)
{
}

ModuloInstruction::~ModuloInstruction()
{
}

void ModuloInstruction::accept(IrVisitor& visitor)
{
	visitor.visit(this);
}

LessInstruction::LessInstruction(Value* result, Value* leftOperand, Value* rightOperand) : ResultInstruction(result), BinaryInstruction(leftOperand, rightOperand)
{
}

LessInstruction::~LessInstruction()
{
}

void LessInstruction::accept(IrVisitor& visitor)
{
	visitor.visit(this);
}

LessEqualInstruction::LessEqualInstruction(Value* result, Value* leftOperand, Value* rightOperand) : ResultInstruction(result), BinaryInstruction(leftOperand, rightOperand)
{
}

LessEqualInstruction::~LessEqualInstruction()
{
}

void LessEqualInstruction::accept(IrVisitor& visitor)
{
	visitor.visit(this);
}

GreaterInstruction::GreaterInstruction(Value* result, Value* leftOperand, Value* rightOperand) : ResultInstruction(result), BinaryInstruction(leftOperand, rightOperand)
{
}

GreaterInstruction::~GreaterInstruction()
{
}

void GreaterInstruction::accept(IrVisitor& visitor)
{
	visitor.visit(this);
}

GreaterEqualInstruction::GreaterEqualInstruction(Value* result, Value* leftOperand, Value* rightOperand) : ResultInstruction(result), BinaryInstruction(leftOperand, rightOperand)
{
}

GreaterEqualInstruction::~GreaterEqualInstruction()
{
}

void GreaterEqualInstruction::accept(IrVisitor& visitor)
{
	visitor.visit(this);
}

EqualInstruction::EqualInstruction(Value* result, Value* leftOperand, Value* rightOperand) : ResultInstruction(result), BinaryInstruction(leftOperand, rightOperand)
{
}

EqualInstruction::~EqualInstruction()
{
}

void EqualInstruction::accept(IrVisitor& visitor)
{
	visitor.visit(this);
}

NotEqualInstruction::NotEqualInstruction(Value* result, Value* leftOperand, Value* rightOperand) : ResultInstruction(result), BinaryInstruction(leftOperand, rightOperand)
{
}

NotEqualInstruction::~NotEqualInstruction()
{
}

void NotEqualInstruction::accept(IrVisitor& visitor)
{
	visitor.visit(this);
}

AndInstruction::AndInstruction(Value* result, Value* leftOperand, Value* rightOperand) : ResultInstruction(result), BinaryInstruction(leftOperand, rightOperand)
{
}

AndInstruction::~AndInstruction()
{
}

void AndInstruction::accept(IrVisitor& visitor)
{
	visitor.visit(this);
}

OrInstruction::OrInstruction(Value* result, Value* leftOperand, Value* rightOperand) : ResultInstruction(result), BinaryInstruction(leftOperand, rightOperand)
{
}

OrInstruction::~OrInstruction()
{
}

void OrInstruction::accept(IrVisitor& visitor)
{
	visitor.visit(this);
}

NotInstruction::NotInstruction(Value* result, Value* operand) : ResultInstruction(result), UnaryInstruction(operand)
{
}

NotInstruction::~NotInstruction()
{
}

void NotInstruction::accept(IrVisitor& visitor)
{
	visitor.visit(this);
}

TypecastInstruction::TypecastInstruction(Value* result, Value* operand) : ResultInstruction(result), UnaryInstruction(operand)
{
}

TypecastInstruction::~TypecastInstruction()
{
}

void TypecastInstruction::accept(IrVisitor& visitor)
{
	visitor.visit(this);
}

} // namespace ir
