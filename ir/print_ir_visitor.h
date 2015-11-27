#ifndef IR_PRINT_IR_VISITOR_H
#define IR_PRINT_IR_VISITOR_H

#include "ir/ir_visitor.h"

namespace ir {

class PrintIrVisitor : public IrVisitor
{
public:
	PrintIrVisitor();
	virtual ~PrintIrVisitor();

	void clear();
	std::string getText() const;

	virtual void visit(Function* func) override;
	virtual void visit(BasicBlock* block) override;
	virtual void visit(NamedValue* value) override;
	virtual void visit(TemporaryValue* value) override;
	virtual void visit(ConstantValue<int>* value) override;
	virtual void visit(ConstantValue<char>* value) override;
	virtual void visit(ConstantValue<std::string>* value) override;
	virtual void visit(AssignInstruction* instr) override;
	virtual void visit(DeclarationInstruction* instr) override;
	virtual void visit(JumpInstruction* instr) override;
	virtual void visit(CondJumpInstruction* instr) override;
	virtual void visit(ReturnInstruction* instr) override;
	virtual void visit(CallInstruction* instr) override;
	virtual void visit(AddInstruction* instr) override;
	virtual void visit(SubtractInstruction* instr) override;
	virtual void visit(MultiplyInstruction* instr) override;
	virtual void visit(DivideInstruction* instr) override;
	virtual void visit(ModuloInstruction* instr) override;
	virtual void visit(LessInstruction* instr) override;
	virtual void visit(LessEqualInstruction* instr) override;
	virtual void visit(GreaterInstruction* instr) override;
	virtual void visit(GreaterEqualInstruction* instr) override;
	virtual void visit(EqualInstruction* instr) override;
	virtual void visit(NotEqualInstruction* instr) override;
	virtual void visit(AndInstruction* instr) override;
	virtual void visit(OrInstruction* instr) override;
	virtual void visit(NotInstruction* instr) override;
	virtual void visit(TypecastInstruction* instr) override;

private:
	std::stringstream _os;
};

} // namespace ir

#endif
