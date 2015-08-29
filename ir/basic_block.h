#ifndef IR_BASIC_BLOCK_H
#define IR_BASIC_BLOCK_H

#include <vector>
#include "ir/instruction.h"

namespace ir {

class BasicBlock
{
public:
    BasicBlock();
    ~BasicBlock();

    std::vector<Instruction*>& getInstructions();
    void addInstruction(Instruction* instruction);

    virtual void text(std::stringstream& os);

    static uint64_t BasicBlockIdPool;
private:
    std::vector<Instruction*> _instructions;
    uint64_t _basicBlockId;
};

} // namespace ir

#endif // IR_BASIC_BLOCK_H
