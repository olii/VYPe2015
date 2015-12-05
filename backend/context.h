#ifndef CONTEXT_H
#define CONTEXT_H
#include "ir/value.h"
#include "ir/instruction.h"
#include "ir/basic_block.h"
#include "mips.h"
#include <map>
#include <string>


namespace backend{

using RegFlag =  struct REGFLAG{ arch::Register * reg; bool saved;};

class FunctionContext;




class BlockContext
{
public:
    BlockContext();
    BlockContext(FunctionContext *parrent, ir::BasicBlock *block);
    ~BlockContext(){}

    void addInstruction(std::string &inst);
    arch::Register *getRegister(ir::NamedValue);
    //arch::Register* getRegister(ir::TemporaryValue);

    std::string getInstructions();

private:
    std::map<ir::Value*, backend::RegFlag> varReg;
    FunctionContext* parrent;
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



private:
    std::map<ir::NamedValue*, int> varStackMap;
    std::map<ir::BasicBlock*, BlockContext> blocks;
    BlockContext *activeBlock;
    int stackCounter = 0;
    ir::Function *func;

    std::string prolog;

    arch::MIPS mips;


};

}

#endif // CONTEXT_H
