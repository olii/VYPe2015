#ifndef FUNCTIONCONTEXT_H
#define FUNCTIONCONTEXT_H


#include <map>
#include <string>
#include <utility>

#include "ir/value.h"
#include "ir/instruction.h"
#include "ir/basic_block.h"
#include "backend/blockcontext.h"
#include "backend/mips.h"
#include "backend/conststringdata.h"



namespace backend{

extern const std::string Indent;

class ASMgenerator;

class FunctionContext
{
public:
    FunctionContext ( FunctionContext && ) = default;
    //FunctionContext ( FunctionContext & ) = default;
    //FunctionContext(){}
    FunctionContext(ir::Function *func, ASMgenerator *parent, const mips::MIPS *mips);

    BlockContext* Active() const;
    void addBlock(const ir::BasicBlock *block);
    void setActiveBlock(const ir::BasicBlock *block);
    const BlockContext *getBlockContext(const ir::BasicBlock *block) const;

    void addVar(ir::NamedValue &var);
    void addVar(ir::NamedValue &var, unsigned paramPos);
    const ir::Function *getFunction() const;
    int getVarOffset(ir::NamedValue &var);
    const std::stringstream getInstructions();
    const mips::MIPS *getMips() const;
    void testCalleeSaved(const mips::Register * reg);

    unsigned int getSpillTableFreePos();
    //std::vector<spillItem> &getSpillTable();
    unsigned int spillTemp(ir::Value *val);
    int unspillTemp(ir::Value *val);
    void cleanspillTable();

    ConstStringData &getStringTable();
    void addInstrSize(int size);
    unsigned getInstrSize();

private:
    std::map<ir::NamedValue*, int> varToStackTable; // map a NamedValue to its place on stack
    std::vector<spillItem> spillTable;
    std::vector<BlockContext> blockContextTable;
    BlockContext *activeBlock;

    int stackCounter = 4; // start on 4 because at 0 there is previous FP

    const ir::Function *func;   // function this context is joined to
    std::stringstream EntryCode; // this will be appended to code right after MIPS function entry
    std::set<const mips::Register*> calleeSavedSet; // set of registers a function has to preserve


    const mips::MIPS *mips;
    ASMgenerator *parent;

    unsigned instrSize = 0;

};

} // namespace backend
#endif // FUNCTIONCONTEXT_H
