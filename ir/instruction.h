#ifndef IR_INSTRUCTION_H
#define IR_INSTRUCTION_H

#include <sstream>
#include <vector>

namespace ir {

class BasicBlock;
class Function;
class Value;

class Instruction
{
public:
    Instruction();
    virtual ~Instruction();

    virtual void text(std::stringstream& os) = 0;
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

    virtual void text(std::stringstream& os) override;
};

class DeclarationInstruction : public UnaryInstruction
{
public:
    DeclarationInstruction(Value* declValue);
    virtual ~DeclarationInstruction();

    virtual void text(std::stringstream& os) override;
};

class JumpInstruction : public Instruction
{
public:
    JumpInstruction(BasicBlock* followingBasicBlock);
    virtual ~JumpInstruction();

    virtual void text(std::stringstream& os) override;

    BasicBlock* getFollowingBasicBlock();

private:
    BasicBlock* _followingBasicBlock;
};

class CondJumpInstruction : public Instruction
{
public:
    CondJumpInstruction(Value* condition, BasicBlock* trueBasicBlock, BasicBlock* falseBasicBlock);
    virtual ~CondJumpInstruction();

    virtual void text(std::stringstream& os) override;

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

    virtual void text(std::stringstream& os) override;
};

class CallInstruction : public ResultInstruction
{
public:
    CallInstruction(Value* result, Function* function, const std::vector<Value*>& arguments);
    virtual ~CallInstruction();

    Function* getFunction();
    std::vector<Value*>& getArguments();

    virtual void text(std::stringstream& os) override;

private:
    Function* _function;
    std::vector<Value*> _arguments;
};

class AddInstruction : public ResultInstruction, public BinaryInstruction
{
public:
    AddInstruction(Value* result, Value* leftOperand, Value* rightOperand);
    virtual ~AddInstruction();

    virtual void text(std::stringstream& os) override;
};

class SubtractInstruction : public ResultInstruction, public BinaryInstruction
{
public:
    SubtractInstruction(Value* result, Value* leftOperand, Value* rightOperand);
    virtual ~SubtractInstruction();

    virtual void text(std::stringstream& os) override;
};

class MultiplyInstruction : public ResultInstruction, public BinaryInstruction
{
public:
    MultiplyInstruction(Value* result, Value* leftOperand, Value* rightOperand);
    virtual ~MultiplyInstruction();

    virtual void text(std::stringstream& os) override;
};

class DivideInstruction : public ResultInstruction, public BinaryInstruction
{
public:
    DivideInstruction(Value* result, Value* leftOperand, Value* rightOperand);
    virtual ~DivideInstruction();

    virtual void text(std::stringstream& os) override;
};

class ModuloInstruction : public ResultInstruction, public BinaryInstruction
{
public:
    ModuloInstruction(Value* result, Value* leftOperand, Value* rightOperand);
    virtual ~ModuloInstruction();

    virtual void text(std::stringstream& os) override;
};

class LessInstruction : public ResultInstruction, public BinaryInstruction
{
public:
    LessInstruction(Value* result, Value* leftOperand, Value* rightOperand);
    virtual ~LessInstruction();

    virtual void text(std::stringstream& os) override;
};

class LessEqualInstruction : public ResultInstruction, public BinaryInstruction
{
public:
    LessEqualInstruction(Value* result, Value* leftOperand, Value* rightOperand);
    virtual ~LessEqualInstruction();

    virtual void text(std::stringstream& os) override;
};

class GreaterInstruction : public ResultInstruction, public BinaryInstruction
{
public:
    GreaterInstruction(Value* result, Value* leftOperand, Value* rightOperand);
    virtual ~GreaterInstruction();

    virtual void text(std::stringstream& os) override;
};

class GreaterEqualInstruction : public ResultInstruction, public BinaryInstruction
{
public:
    GreaterEqualInstruction(Value* result, Value* leftOperand, Value* rightOperand);
    virtual ~GreaterEqualInstruction();

    virtual void text(std::stringstream& os) override;
};

class EqualInstruction : public ResultInstruction, public BinaryInstruction
{
public:
    EqualInstruction(Value* result, Value* leftOperand, Value* rightOperand);
    virtual ~EqualInstruction();

    virtual void text(std::stringstream& os) override;
};

class NotEqualInstruction : public ResultInstruction, public BinaryInstruction
{
public:
    NotEqualInstruction(Value* result, Value* leftOperand, Value* rightOperand);
    virtual ~NotEqualInstruction();

    virtual void text(std::stringstream& os) override;
};

class AndInstruction : public ResultInstruction, public BinaryInstruction
{
public:
    AndInstruction(Value* result, Value* leftOperand, Value* rightOperand);
    virtual ~AndInstruction();

    virtual void text(std::stringstream& os) override;
};

class OrInstruction : public ResultInstruction, public BinaryInstruction
{
public:
    OrInstruction(Value* result, Value* leftOperand, Value* rightOperand);
    virtual ~OrInstruction();

    virtual void text(std::stringstream& os) override;
};

class NotInstruction : public ResultInstruction, public UnaryInstruction
{
public:
    NotInstruction(Value* result, Value* operand);
    virtual ~NotInstruction();

    virtual void text(std::stringstream& os) override;
};

} // namespace ir

#endif // IR_INSTRUCTION_H
