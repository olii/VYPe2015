#ifndef CONTEXT_H
#define CONTEXT_H
#include "ir/value.h"
#include "ir/instruction.h"
#include "ir/basic_block.h"
#include "mips.h"
#include <map>
#include <string>
#include <cstdarg>


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

    void addCanonicalInstruction(std::string &inst);
    void addInstruction(std::string inst,std::string dst, int offset = 0, std::string op2 = "",int offset2 = 0,std::string op3 = "");

    arch::Register *getRegister(ir::Value *val, bool locked = false);
    void unlockVar(ir::Value *val);
    void removeVictim();

    std::string getInstructions();

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
