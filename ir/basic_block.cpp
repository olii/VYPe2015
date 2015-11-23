#include "ir/basic_block.h"

namespace ir {

uint64_t BasicBlock::BasicBlockIdPool = 0;

BasicBlock::BasicBlock() : _instructions(), _basicBlockId(BasicBlockIdPool++), _defs(), _uses(), _pred(), _succ()
{
}

BasicBlock::~BasicBlock()
{
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

void BasicBlock::text(std::stringstream& os)
{
	os << "@" << _basicBlockId;
}

void BasicBlock::detail(std::stringstream& os)
{
	bool any = false;
	std::string prefix = "pred[";
	for (BasicBlock* pred : _pred)
	{
		any = true;
		os << prefix;
		pred->text(os);
		prefix = ", ";
	}
	os << (any ? "" : prefix) << "] ";

	any = false;
	prefix = "succ[";
	for (BasicBlock* succ : _succ)
	{
		any = true;
		os << prefix;
		succ->text(os);
		prefix = ", ";
	}
	os << (any ? "" : prefix) << "] ";

	any = false;
	prefix = "defs[";
	for (Value* def : _defs)
	{
		any = true;
		os << prefix;
		def->text(os);
		prefix = ", ";
	}
	os << (any ? "" : prefix) << "] ";

	any = false;
	prefix = "uses[";
	for (Value* use : _uses)
	{
		any = true;
		os << prefix;
		use->text(os);
		prefix = ", ";
	}
	os << (any ? "" : prefix) << "]";
}

} // namespace ir
