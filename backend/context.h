#ifndef CONTEXT_H
#define CONTEXT_H
#include "ir/value.h"
#include "ir/instruction.h"
#include "ir/basic_block.h"
#include "mips.h"
#include <map>
#include <string>
#include <cstdarg>
#include <utility>


namespace backend{

using RegFlag =  struct REGFLAG{ arch::Register * reg; bool saved; bool locked;};
using VarFlag = struct VARFLAG{ ir::Value * val; bool saved;};

class FunctionContext;




class BlockContext
{
public:
    BlockContext();
    BlockContext(FunctionContext *parent, ir::BasicBlock *block);
    ~BlockContext(){}

    void addCanonicalInstruction(const std::string &inst);
    void addInstruction(const std::string &inst, const std::string &dst, int offset = 0, const std::string &op2 = "", int offset2 = 0, const std::string &op3 = "");

    arch::Register *getRegister(ir::Value *val, bool locked = false, bool implicitCopy = true);
    arch::Register *getFreeRegister();

    void unlockVar(ir::Value *val);
    void removeVictim();
    void markChanged(arch::Register * reg);

    const std::vector<std::pair<ir::Value *, int> > saveCallerRegisters();
    void clearCallerRegisters();
    void loadMapingAfterCall(const std::vector<std::pair<ir::Value* , int >> &map);
    void saveUnsavedVariables();

    const std::string getInstructions();
    const std::string getName();


    static const std::string getName(ir::BasicBlock *b, ir::Function *f);

private:
    std::map<ir::Value*, backend::RegFlag> varToReg;
    std::map<arch::Register*, VarFlag> RegToVar;
    FunctionContext* parent;
    ir::BasicBlock *block;

    std::string text;
};




class FunctionContext
{
public:
    static const std::string Indentation;


    FunctionContext(){}
    FunctionContext(ir::Function *func);

    BlockContext* Active();
    void addBlock(ir::BasicBlock *block);
    void addVar(ir::NamedValue &var);
    void addVar(ir::NamedValue &var, unsigned paramPos, unsigned paramAll);

    ir::Function *getFunction();

    int getVarOffset(ir::NamedValue &var);
    std::string getInstructions();

    arch::MIPS *getMips();

    void calleeSaved(arch::Register * reg);



private:
    std::map<ir::NamedValue*, int> varStackMap;
    std::map<ir::BasicBlock*, BlockContext> blocks;
    BlockContext *activeBlock;
    int stackCounter = 4; // start on 4 because at 0 there is previous FP
    ir::Function *func;

    std::string prolog;
    std::set<arch::Register*> calleeSavedSet;


    arch::MIPS mips;


};

}

#endif // CONTEXT_H
