#ifndef IR_BASIC_BLOCK_H
#define IR_BASIC_BLOCK_H

#include <set>
#include <vector>
#include "ir/instruction.h"
#include "ir/value.h"

namespace ir {

class IrVisitor;

class BasicBlock
{
public:
	BasicBlock();
	~BasicBlock();

	void accept(IrVisitor& visitor);

	uint64_t getId() const;

	Instruction* getTerminalInstruction() const;
	std::vector<Instruction*>& getInstructions();
	void addInstruction(Instruction* instruction);

	const std::set<Value*>& getDefs() const;
	void addDef(Value* value);

	const std::set<Value*>& getUses() const;
	void addUse(Value* value);

	std::set<BasicBlock*>& getPredecessors();
	void addPredecessor(BasicBlock* basicBlock);

	std::set<BasicBlock*>& getSuccessors();
	void addSuccessor(BasicBlock* basicBlock);

	static uint64_t BasicBlockIdPool;
private:
	std::vector<Instruction*> _instructions;
	uint64_t _basicBlockId;
	std::set<Value*> _defs;
	std::set<Value*> _uses;
	std::set<BasicBlock*> _pred;
	std::set<BasicBlock*> _succ;
};

} // namespace ir

#endif // IR_BASIC_BLOCK_H
