#include "ir/basic_block.h"

namespace ir {

uint64_t BasicBlock::BasicBlockIdPool = 0;

BasicBlock::BasicBlock() : _instructions(), _basicBlockId(BasicBlockIdPool++)
{
}

BasicBlock::~BasicBlock()
{
}

std::vector<Instruction*>& BasicBlock::getInstructions()
{
    return _instructions;
}

void BasicBlock::addInstruction(Instruction* instruction)
{
    _instructions.push_back(instruction);
}

void BasicBlock::text(std::stringstream& os)
{
    os << "@" << _basicBlockId;
}

} // namespace ir
