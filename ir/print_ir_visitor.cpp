#include "ir/function.h"
#include "ir/print_ir_visitor.h"

namespace ir {

static const std::string Indentation = "      ";

PrintIrVisitor::PrintIrVisitor()
{
}

PrintIrVisitor::~PrintIrVisitor()
{
}

void PrintIrVisitor::clear()
{
	_os.str(std::string());
}

std::string PrintIrVisitor::getText() const
{
	return _os.str();
}

void PrintIrVisitor::visit(Function* func)
{
	_os << "function " << func->getName() << "(";

	const std::vector<Value*> parameters = func->getParameters();
	for (uint32_t i = 0; i < parameters.size(); ++i)
	{
		parameters[i]->accept(*this);
		_os << ":" << Value::dataTypeToString(parameters[i]->getDataType());

		// Make sure there is no trailing , at the end of parameters
		if (i != parameters.size() - 1)
			_os << ", ";
	}
	_os << "):" << Value::dataTypeToString(func->getReturnDataType()) << "\n";

	for (BasicBlock* bb : func->getBasicBlocks())
	{
		_os << "@" << bb->getId() << ": ";
		bb->accept(*this);
		_os << "\n";

		for (Instruction* inst : bb->getInstructions())
			inst->accept(*this);
	}

	_os << "\n";
}

void PrintIrVisitor::visit(BasicBlock* block)
{
	bool any = false;
	std::string prefix = "pred[";
	for (BasicBlock* pred : block->getPredecessors())
	{
		any = true;
		_os << prefix << "@" << pred->getId();
		prefix = ", ";
	}
	_os << (any ? "" : prefix) << "] ";

	any = false;
	prefix = "succ[";
	for (BasicBlock* succ : block->getSuccessors())
	{
		any = true;
		_os << prefix << "@" << succ->getId();
		prefix = ", ";
	}
	_os << (any ? "" : prefix) << "] ";

	any = false;
	prefix = "defs[";
	for (Value* def : block->getDefs())
	{
		any = true;
		_os << prefix;
		def->accept(*this);
		prefix = ", ";
	}
	_os << (any ? "" : prefix) << "] ";

	any = false;
	prefix = "uses[";
	for (Value* use : block->getUses())
	{
		any = true;
		_os << prefix;
		use->accept(*this);
		prefix = ", ";
	}
	_os << (any ? "" : prefix) << "]";
}

void PrintIrVisitor::visit(NamedValue* value)
{
	_os << value->getName();
}

void PrintIrVisitor::visit(TemporaryValue* value)
{
	_os << value->getSymbolicName();
}

void PrintIrVisitor::visit(ConstantValue<int>* value)
{
	_os << value->getConstantValue();
}

void PrintIrVisitor::visit(ConstantValue<char>* value)
{
	_os << "\'" << value->getConstantValue() << "\'";
}

void PrintIrVisitor::visit(ConstantValue<std::string>* value)
{
	_os << "\"" << value->getConstantValue() << "\"";
}

void PrintIrVisitor::visit(AssignInstruction* instr)
{
	_os << Indentation;
	instr->getResult()->accept(*this);
	_os << " = ";
	instr->getOperand()->accept(*this);
	_os << "\n";
}

void PrintIrVisitor::visit(DeclarationInstruction* instr)
{
	_os << Indentation;
	_os << "declare ";
	instr->getOperand()->accept(*this);
	_os << ":" << Value::dataTypeToString(instr->getOperand()->getDataType()) << "\n";
}

void PrintIrVisitor::visit(JumpInstruction* instr)
{
	_os << Indentation;
	_os << "jump @" << instr->getFollowingBasicBlock()->getId() << "\n";
}

void PrintIrVisitor::visit(CondJumpInstruction* instr)
{
	_os << Indentation;
	_os << "jumpif ";
	instr->getCondition()->accept(*this);
	_os << ", @" << instr->getTrueBasicBlock()->getId() << ", @" << instr->getFalseBasicBlock()->getId() << "\n";
}

void PrintIrVisitor::visit(ReturnInstruction* instr)
{
	_os << Indentation;
	_os << "return";
	if (instr->getOperand() != nullptr)
	{
		_os << " ";
		instr->getOperand()->accept(*this);
	}
	_os << "\n";
}

void PrintIrVisitor::visit(CallInstruction* instr)
{
	_os << Indentation;
	instr->getResult()->accept(*this);
	_os << " = call " << instr->getFunction()->getName();
	for (Value* arg : instr->getArguments())
	{
		_os << ", ";
		arg->accept(*this);
	}
	_os << "\n";
}

void PrintIrVisitor::visit(AddInstruction* instr)
{
	_os << Indentation;
	instr->getResult()->accept(*this);
	_os << " = add ";
	instr->getLeftOperand()->accept(*this);
	_os << ", ";
	instr->getRightOperand()->accept(*this);
	_os << "\n";
}

void PrintIrVisitor::visit(SubtractInstruction* instr)
{
	_os << Indentation;
	instr->getResult()->accept(*this);
	_os << " = subtract ";
	instr->getLeftOperand()->accept(*this);
	_os << ", ";
	instr->getRightOperand()->accept(*this);
	_os << "\n";
}

void PrintIrVisitor::visit(MultiplyInstruction* instr)
{
	_os << Indentation;
	instr->getResult()->accept(*this);
	_os << " = multiply ";
	instr->getLeftOperand()->accept(*this);
	_os << ", ";
	instr->getRightOperand()->accept(*this);
	_os << "\n";
}

void PrintIrVisitor::visit(DivideInstruction* instr)
{
	_os << Indentation;
	instr->getResult()->accept(*this);
	_os << " = divide ";
	instr->getLeftOperand()->accept(*this);
	_os << ", ";
	instr->getRightOperand()->accept(*this);
	_os << "\n";
}

void PrintIrVisitor::visit(ModuloInstruction* instr)
{
	_os << Indentation;
	instr->getResult()->accept(*this);
	_os << " = modulo ";
	instr->getLeftOperand()->accept(*this);
	_os << ", ";
	instr->getRightOperand()->accept(*this);
	_os << "\n";
}

void PrintIrVisitor::visit(LessInstruction* instr)
{
	_os << Indentation;
	instr->getResult()->accept(*this);
	_os << " = less ";
	instr->getLeftOperand()->accept(*this);
	_os << ", ";
	instr->getRightOperand()->accept(*this);
	_os << "\n";
}

void PrintIrVisitor::visit(LessEqualInstruction* instr)
{
	_os << Indentation;
	instr->getResult()->accept(*this);
	_os << " = lesseq ";
	instr->getLeftOperand()->accept(*this);
	_os << ", ";
	instr->getRightOperand()->accept(*this);
	_os << "\n";
}

void PrintIrVisitor::visit(GreaterInstruction* instr)
{
	_os << Indentation;
	instr->getResult()->accept(*this);
	_os << " = greater ";
	instr->getLeftOperand()->accept(*this);
	_os << ", ";
	instr->getRightOperand()->accept(*this);
	_os << "\n";
}

void PrintIrVisitor::visit(GreaterEqualInstruction* instr)
{
	_os << Indentation;
	instr->getResult()->accept(*this);
	_os << " = greatereq ";
	instr->getLeftOperand()->accept(*this);
	_os << ", ";
	instr->getRightOperand()->accept(*this);
	_os << "\n";
}

void PrintIrVisitor::visit(EqualInstruction* instr)
{
	_os << Indentation;
	instr->getResult()->accept(*this);
	_os << " = equal ";
	instr->getLeftOperand()->accept(*this);
	_os << ", ";
	instr->getRightOperand()->accept(*this);
	_os << "\n";
}

void PrintIrVisitor::visit(NotEqualInstruction* instr)
{
	_os << Indentation;
	instr->getResult()->accept(*this);
	_os << " = notequal ";
	instr->getLeftOperand()->accept(*this);
	_os << ", ";
	instr->getRightOperand()->accept(*this);
	_os << "\n";
}

void PrintIrVisitor::visit(AndInstruction* instr)
{
	_os << Indentation;
	instr->getResult()->accept(*this);
	_os << " = and ";
	instr->getLeftOperand()->accept(*this);
	_os << ", ";
	instr->getRightOperand()->accept(*this);
	_os << "\n";
}

void PrintIrVisitor::visit(OrInstruction* instr)
{
	_os << Indentation;
	instr->getResult()->accept(*this);
	_os << " = or ";
	instr->getLeftOperand()->accept(*this);
	_os << ", ";
	instr->getRightOperand()->accept(*this);
	_os << "\n";
}

void PrintIrVisitor::visit(NotInstruction* instr)
{
	_os << Indentation;
	instr->getResult()->accept(*this);
	_os << " = not ";
	instr->getOperand()->accept(*this);
	_os << "\n";
}

} // namespace ir
