#ifndef IR_IR_VISITOR_H
#define IR_IR_VISITOR_H

#include "ir/instruction.h"
#include "ir/value.h"

namespace ir {

class IrVisitor
{
public:
	virtual ~IrVisitor() {}

	virtual void visit(Function* func) = 0;
	virtual void visit(BasicBlock* block) = 0;
	virtual void visit(NamedValue* value) = 0;
	virtual void visit(TemporaryValue* value) = 0;
	virtual void visit(ConstantValue<int>* value) = 0;
	virtual void visit(ConstantValue<char>* value) = 0;
	virtual void visit(ConstantValue<std::string>* value) = 0;
	virtual void visit(AssignInstruction* instr) = 0;
	virtual void visit(DeclarationInstruction* instr) = 0;
	virtual void visit(JumpInstruction* instr) = 0;
	virtual void visit(CondJumpInstruction* instr) = 0;
	virtual void visit(ReturnInstruction* instr) = 0;
	virtual void visit(CallInstruction* instr) = 0;
	virtual void visit(AddInstruction* instr) = 0;
	virtual void visit(SubtractInstruction* instr) = 0;
	virtual void visit(MultiplyInstruction* instr) = 0;
	virtual void visit(DivideInstruction* instr) = 0;
	virtual void visit(ModuloInstruction* instr) = 0;
	virtual void visit(LessInstruction* instr) = 0;
	virtual void visit(LessEqualInstruction* instr) = 0;
	virtual void visit(GreaterInstruction* instr) = 0;
	virtual void visit(GreaterEqualInstruction* instr) = 0;
	virtual void visit(EqualInstruction* instr) = 0;
	virtual void visit(NotEqualInstruction* instr) = 0;
	virtual void visit(AndInstruction* instr) = 0;
	virtual void visit(OrInstruction* instr) = 0;
	virtual void visit(NotInstruction* instr) = 0;

protected:
	IrVisitor() {}
	IrVisitor(const IrVisitor&) {}
};

} // namespace ir

#endif
