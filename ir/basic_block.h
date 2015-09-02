#ifndef IR_BASIC_BLOCK_H
#define IR_BASIC_BLOCK_H

#include <set>
#include <vector>
#include "ir/instruction.h"
#include "ir/value.h"

namespace ir {

class BasicBlock
{
public:
    BasicBlock();
    ~BasicBlock();

    std::vector<Instruction*>& getInstructions();
    void addInstruction(Instruction* instruction);

    const std::set<Value*>& getDefs() const;
    void addDef(Value* value);

    const std::set<Value*>& getUses() const;
    void addUse(Value* value);

    virtual void text(std::stringstream& os);
    virtual void detail(std::stringstream& os);

    static uint64_t BasicBlockIdPool;
private:
    std::vector<Instruction*> _instructions;
    uint64_t _basicBlockId;
    std::set<Value*> _defs;
    std::set<Value*> _uses;
};

} // namespace ir

#endif // IR_BASIC_BLOCK_H
