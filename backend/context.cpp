#include "context.h"
#include "ir/function.h"

namespace backend{


BlockContext::BlockContext()
{

}

BlockContext::BlockContext(backend::FunctionContext *parrent, ir::BasicBlock *block)
{
    this->parrent = parrent;
    this->block = block;
}

void BlockContext::addInstruction(std::string &inst)
{
    text += inst + "\n";
}

arch::Register *BlockContext::getRegister(ir::NamedValue)
{
    return nullptr;
}

std::string BlockContext::getInstructions()
{
    std::string instr;

    //comment
    instr += FunctionContext::Indentation +  "#BLOCK " + std::to_string(block->getId()) + "\n";

    //label
    instr += FunctionContext::Indentation + parrent->getFunction()->getName() + "_" + std::to_string(block->getId()) + " : \n";

    instr += text;

    return instr;
}








const std::string FunctionContext::Indentation = "      ";

FunctionContext::FunctionContext(ir::Function *func)
{
    this->func = func;
}

BlockContext *FunctionContext::Active()
{
    return activeBlock;
}

void FunctionContext::addBlock(ir::BasicBlock *block)
{
    blocks[block] = BlockContext(this, block);
    activeBlock  = &blocks[block];
}

void FunctionContext::addVar(ir::NamedValue &var)
{
    varStackMap[&var] = stackCounter;
    stackCounter += 4; // add stack offset, 4 bytes
}

void FunctionContext::addVar(ir::NamedValue &var, unsigned paramPos, unsigned paramAll)
{
    bool regMapped = false;
    if (paramPos <= mips.getMaxRegisterParameters()){
        regMapped = true;
    }


    addVar(var);
    unsigned localOffset = getVarOffset(var);

    if (regMapped)
    {
        //copy from reg to local stack
        arch::Register *src = mips.getParamRegisters()[paramPos];
        prolog += Indentation + "sw " +  src->getName() +", (" + std::to_string(localOffset) + ")$sp"  + "\n"; ;
    } else {
        // copy from stack to local stack using PARAMregister 0
        arch::Register *src = mips.getParamRegisters()[0];
        int fpOff = (paramAll - paramPos + 1); // compute relative offset to Frame Pointer
        int fpOffBytes = 4 + 4 * fpOff; // skip the RA

        // load and store
        prolog += Indentation + "lw " + src->getName() + ", " + std::to_string(fpOffBytes)  +"($fp)" + "\n";
        prolog += Indentation + "sw " +  src->getName() +", " + std::to_string(localOffset) + "($sp)";

    }
}

ir::Function *FunctionContext::getFunction()
{
    return func;
}

int FunctionContext::getVarOffset(ir::NamedValue &var)
{
    if ( varStackMap.find(&var) == varStackMap.end() ) {
      return -1;
    } else {
      return varStackMap[&var];
    }
}

std::string FunctionContext::getInstructions()
{
    std::string instr;

    instr += "#FUNCTION " + func->getName() + "\n";
    instr += func->getName() + " : \n";              // label

    instr += mips.getFunctionPrologue();
    instr += "sub $sp, $sp, " + std::to_string(varStackMap.size()) + " \n" ;
    instr += prolog;


    for(auto &it : blocks){
        instr +=  it.second.getInstructions();
    }
    return instr;
}


}
