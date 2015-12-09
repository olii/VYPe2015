#ifndef IR_INSTRUCTION_H
#define IR_INSTRUCTION_H

#include <sstream>
#include <vector>

namespace ir {

class BasicBlock;
class Function;
class IrVisitor;
class Value;

class Instruction
{
public:
	Instruction();
	virtual ~Instruction();

	virtual void accept(IrVisitor& visitor) = 0;
};

class UnaryInstruction : public virtual Instruction
{
public:
	UnaryInstruction(Value* operand);
	virtual ~UnaryInstruction();

	Value* getOperand();

private:
	Value* _operand;
};

class BinaryInstruction : public virtual Instruction
{
public:
	BinaryInstruction(Value* leftOperand, Value* rightOperand);
	virtual ~BinaryInstruction();

	Value* getLeftOperand();
	Value* getRightOperand();

private:
	Value* _leftOperand;
	Value* _rightOperand;
};

class ResultInstruction : public virtual Instruction
{
public:
	ResultInstruction(Value* result);
	virtual ~ResultInstruction();

	Value* getResult();

private:
	Value* _result;
};

class AssignInstruction : public ResultInstruction, public UnaryInstruction
{
public:
	AssignInstruction(Value* dest, Value* value);
	virtual ~AssignInstruction();

	virtual void accept(IrVisitor& visitor) override;
};

class DeclarationInstruction : public UnaryInstruction
{
public:
	DeclarationInstruction(Value* declValue);
	virtual ~DeclarationInstruction();

	virtual void accept(IrVisitor& visitor) override;
};

class JumpInstruction : public Instruction
{
public:
	JumpInstruction(BasicBlock* followingBasicBlock);
	virtual ~JumpInstruction();

	virtual void accept(IrVisitor& visitor) override;

	BasicBlock* getFollowingBasicBlock();

private:
	BasicBlock* _followingBasicBlock;
};

class CondJumpInstruction : public Instruction
{
public:
	CondJumpInstruction(Value* condition, BasicBlock* trueBasicBlock, BasicBlock* falseBasicBlock);
	virtual ~CondJumpInstruction();

	virtual void accept(IrVisitor& visitor) override;

	Value* getCondition();
	BasicBlock* getTrueBasicBlock();
	BasicBlock* getFalseBasicBlock();

protected:
	Value* _condition;
	BasicBlock* _trueBasicBlock;
	BasicBlock* _falseBasicBlock;
};

class ReturnInstruction : public UnaryInstruction
{
public:
	ReturnInstruction(Value* returnValue);
	virtual ~ReturnInstruction();

	virtual void accept(IrVisitor& visitor) override;
};

class CallInstruction : public ResultInstruction
{
public:
	CallInstruction(Value* result, Function* function, const std::vector<Value*>& arguments);
	virtual ~CallInstruction();

	virtual void accept(IrVisitor& visitor) override;

	Function* getFunction();
	std::vector<Value*>& getArguments();

private:
	Function* _function;
	std::vector<Value*> _arguments;
};

class BuiltinCallInstruction : public ResultInstruction
{
public:
	BuiltinCallInstruction(Value* result, const std::string& functionName, const std::vector<Value*>& arguments);
	virtual ~BuiltinCallInstruction();

	virtual void accept(IrVisitor& visitor) override;

	const std::string& getFunctionName() const;
	std::vector<Value*>& getArguments();

private:
	std::string _functionName;
	std::vector<Value*> _arguments;
};

class AddInstruction : public ResultInstruction, public BinaryInstruction
{
public:
	AddInstruction(Value* result, Value* leftOperand, Value* rightOperand);
	virtual ~AddInstruction();

	virtual void accept(IrVisitor& visitor) override;
};

class SubtractInstruction : public ResultInstruction, public BinaryInstruction
{
public:
	SubtractInstruction(Value* result, Value* leftOperand, Value* rightOperand);
	virtual ~SubtractInstruction();

	virtual void accept(IrVisitor& visitor) override;
};

class MultiplyInstruction : public ResultInstruction, public BinaryInstruction
{
public:
	MultiplyInstruction(Value* result, Value* leftOperand, Value* rightOperand);
	virtual ~MultiplyInstruction();

	virtual void accept(IrVisitor& visitor) override;
};

class DivideInstruction : public ResultInstruction, public BinaryInstruction
{
public:
	DivideInstruction(Value* result, Value* leftOperand, Value* rightOperand);
	virtual ~DivideInstruction();

	virtual void accept(IrVisitor& visitor) override;
};

class ModuloInstruction : public ResultInstruction, public BinaryInstruction
{
public:
	ModuloInstruction(Value* result, Value* leftOperand, Value* rightOperand);
	virtual ~ModuloInstruction();

	virtual void accept(IrVisitor& visitor) override;
};

class LessInstruction : public ResultInstruction, public BinaryInstruction
{
public:
	LessInstruction(Value* result, Value* leftOperand, Value* rightOperand);
	virtual ~LessInstruction();

	virtual void accept(IrVisitor& visitor) override;
};

class LessEqualInstruction : public ResultInstruction, public BinaryInstruction
{
public:
	LessEqualInstruction(Value* result, Value* leftOperand, Value* rightOperand);
	virtual ~LessEqualInstruction();

	virtual void accept(IrVisitor& visitor) override;
};

class GreaterInstruction : public ResultInstruction, public BinaryInstruction
{
public:
	GreaterInstruction(Value* result, Value* leftOperand, Value* rightOperand);
	virtual ~GreaterInstruction();

	virtual void accept(IrVisitor& visitor) override;
};

class GreaterEqualInstruction : public ResultInstruction, public BinaryInstruction
{
public:
	GreaterEqualInstruction(Value* result, Value* leftOperand, Value* rightOperand);
	virtual ~GreaterEqualInstruction();

	virtual void accept(IrVisitor& visitor) override;
};

class EqualInstruction : public ResultInstruction, public BinaryInstruction
{
public:
	EqualInstruction(Value* result, Value* leftOperand, Value* rightOperand);
	virtual ~EqualInstruction();

	virtual void accept(IrVisitor& visitor) override;
};

class NotEqualInstruction : public ResultInstruction, public BinaryInstruction
{
public:
	NotEqualInstruction(Value* result, Value* leftOperand, Value* rightOperand);
	virtual ~NotEqualInstruction();

	virtual void accept(IrVisitor& visitor) override;
};

class AndInstruction : public ResultInstruction, public BinaryInstruction
{
public:
	AndInstruction(Value* result, Value* leftOperand, Value* rightOperand);
	virtual ~AndInstruction();

	virtual void accept(IrVisitor& visitor) override;
};

class OrInstruction : public ResultInstruction, public BinaryInstruction
{
public:
	OrInstruction(Value* result, Value* leftOperand, Value* rightOperand);
	virtual ~OrInstruction();

	virtual void accept(IrVisitor& visitor) override;
};

class BitwiseAndInstruction : public ResultInstruction, public BinaryInstruction
{
public:
	BitwiseAndInstruction(Value* result, Value* leftOperand, Value* rightOperand);
	virtual ~BitwiseAndInstruction();

	virtual void accept(IrVisitor& visitor) override;
};

class BitwiseOrInstruction : public ResultInstruction, public BinaryInstruction
{
public:
	BitwiseOrInstruction(Value* result, Value* leftOperand, Value* rightOperand);
	virtual ~BitwiseOrInstruction();

	virtual void accept(IrVisitor& visitor) override;
};

class NotInstruction : public ResultInstruction, public UnaryInstruction
{
public:
	NotInstruction(Value* result, Value* operand);
	virtual ~NotInstruction();

	virtual void accept(IrVisitor& visitor) override;
};

class TypecastInstruction : public ResultInstruction, public UnaryInstruction
{
public:
	TypecastInstruction(Value* result, Value* operand);
	virtual ~TypecastInstruction();

	virtual void accept(IrVisitor& visitor) override;
};

class BitwiseNotInstruction : public ResultInstruction, public UnaryInstruction
{
public:
	BitwiseNotInstruction(Value* result, Value* operand);
	virtual ~BitwiseNotInstruction();

	virtual void accept(IrVisitor& visitor) override;
};

class NegInstruction : public ResultInstruction, public UnaryInstruction
{
public:
	NegInstruction(Value* result, Value* operand);
	virtual ~NegInstruction();

	virtual void accept(IrVisitor& visitor) override;
};

} // namespace ir

#endif // IR_INSTRUCTION_H
