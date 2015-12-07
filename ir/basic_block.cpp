#include "ir/basic_block.h"
#include "ir/ir_visitor.h"

namespace ir {

uint64_t BasicBlock::BasicBlockIdPool = 0;

BasicBlock::BasicBlock() : _instructions(), _basicBlockId(BasicBlockIdPool++), _defs(), _uses(), _pred(), _succ()
{
}

BasicBlock::~BasicBlock()
{
	for (auto& inst : _instructions)
		delete inst;
}

void BasicBlock::accept(IrVisitor& visitor)
{
	visitor.visit(this);
}

uint64_t BasicBlock::getId() const
{
	return _basicBlockId;
}

Instruction* BasicBlock::getTerminalInstruction() const
{
	if (_instructions.empty())
		return nullptr;

	return _instructions.back();
}

std::vector<Instruction*>& BasicBlock::getInstructions()
{
	return _instructions;
}

const std::set<Value*>& BasicBlock::getDefs() const
{
	return _defs;
}

void BasicBlock::addDef(Value* value)
{
	_defs.insert(value);
}

const std::set<Value*>& BasicBlock::getUses() const
{
	return _uses;
}

void BasicBlock::addUse(Value* value)
{
	_uses.insert(value);
}

std::set<BasicBlock*>& BasicBlock::getPredecessors()
{
	return _pred;
}

void BasicBlock::addPredecessor(BasicBlock* basicBlock)
{
	_pred.insert(basicBlock);
}

std::set<BasicBlock*>& BasicBlock::getSuccessors()
{
	return _succ;
}

void BasicBlock::addSuccessor(BasicBlock* basicBlock)
{
	_succ.insert(basicBlock);
}

void BasicBlock::addInstruction(Instruction* instruction)
{
	_instructions.push_back(instruction);
}

} // namespace ir
