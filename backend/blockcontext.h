#ifndef BLOCKCONTEXT_H
#define BLOCKCONTEXT_H

#include <map>
#include <string>
#include <utility>

#include "ir/value.h"
#include "ir/instruction.h"
#include "ir/basic_block.h"
#include "backend/mips.h"


namespace backend{
class FunctionContext;

//#define QTCreator_IDE 1
// to avoid parsing "using typedefs" because qtcreator has no support for this



//using RegFlag = struct REGFLAG{ mips::Register * reg; bool saved; bool locked;};
//using VarFlag = struct VARFLAG{ ir::Value * val; bool saved;};

struct registerItem{
    const mips::Register *reg;
    ir::Value *val = nullptr;
    bool saved;
    unsigned int lruValue;
};

struct spillItem{
    ir::Value *val = nullptr;
    unsigned int offset;
};


class BlockContext
{
public:
    BlockContext ( BlockContext && ) = default;
    BlockContext(FunctionContext *Parent, const ir::BasicBlock *Block);
    ~BlockContext();

    const std::string getName() const;
    const std::string getInstructions() const ;
    const mips::MIPS *getMips() const ;

    const ir::BasicBlock *getBlock() const;

    void addCanonicalInstruction(const std::string &inst);
    void addLabel(const std::string &label);
    void addInstruction(const std::string &inst, const mips::Register &reg);
    void addInstruction(const std::string &inst, const mips::Register &dst, const mips::Register &src);
    void addInstruction(const std::string &inst, const mips::Register &dst, const int imm);
    void addInstruction(const std::string &inst, const ir::BasicBlock *block);
    void addInstruction(const std::string &inst, const ir::Function *func);
    void addInstruction(const std::string &inst, const mips::Register &dst , const int imm, const mips::Register &src);
    void addInstruction(const std::string &inst, const mips::Register &dst , const mips::Register &op1, const int imm);
    void addInstruction(const std::string &inst, const mips::Register &op1 , const mips::Register &op2, const ir::BasicBlock *block);
    void addInstruction(const std::string &inst, const mips::Register &dst , const mips::Register &op1, const mips::Register &op2);
    void addInstruction(const std::string &inst, const mips::Register &dst , const mips::Register &op1, const int imm, const mips::Register &op2);

    std::vector<std::pair<ir::Value *, int> > saveCallerRegisters();
    void saveUnsavedVariables();
    void saveTemporaries();
    void clearCallerRegisters();

    const mips::Register *getRegister(ir::Value *val, bool load = true);
    const mips::Register *getFreeRegister();
    void markChanged(const mips::Register *reg);

    void updateLRU();
/*

    std::vector<std::pair<ir::Value *, int> > saveCallerRegisters();
    void clearCallerRegisters();
    void loadMapingAfterCall(const std::vector<std::pair<ir::Value* , int >> &map, int size);
    void saveUnsavedVariables();

    static const std::string getName(ir::BasicBlock *b, ir::Function *f);
*/

private:
    FunctionContext* parent;
    const ir::BasicBlock *block;

    std::vector<registerItem> registerTable;

    std::stringstream text;

    registerItem *getFreeTableItem();
    void removeVictim();

};

} // namespace end

#endif // BLOCKCONTEXT_H
