/**
 * Project VYPe 2015/2016
 * Marek Milkovič (xmilko01) & Oliver Nemček (xnemce03)
 **/

#include "functioncontext.h"
#include "ir/function.h"
#include "backend/asmgenerator.h"


namespace backend{

extern const std::string Indent = "        ";


FunctionContext::FunctionContext(ir::Function *func, ASMgenerator *parent, const mips::MIPS *mips)
{
    this->func = func;
    this->mips = mips;
    this->parent = parent;
}

BlockContext *FunctionContext::Active() const
{
    return activeBlock;
}

void FunctionContext::addBlock(const ir::BasicBlock *block)
{
    for(auto &iter : blockContextTable)
    {
        if (iter.getBlock() == block)
        {
            return;
        }
    }
    blockContextTable.emplace_back(std::move(BlockContext(this, block)));
}

void FunctionContext::setActiveBlock(const ir::BasicBlock *block)
{
    for(auto &iter : blockContextTable)
    {
        if (iter.getBlock() == block)
        {
            activeBlock  = &iter;
            return;
        }
    }
}

const BlockContext *FunctionContext::getBlockContext(const ir::BasicBlock *block) const
{
    for(auto &iter : blockContextTable)
    {
        if (iter.getBlock() == block)
        {
            return &iter;
        }
    }
    return nullptr;
}

void FunctionContext::addVar(ir::NamedValue &var)
{
    //if aleady on stack
    if (varToStackTable.find(&var) != varToStackTable.end()) return;

    varToStackTable[&var] = stackCounter;
    EntryCode << Indent << "  #Variable " << var.getName() << " got offset: " << stackCounter << " ... " << -stackCounter << "($fp)" << "\n";
    stackCounter += 4; // add stack offset, 4 bytes
}

void FunctionContext::addVar(ir::NamedValue &var, unsigned paramPos)
{
    unsigned maxRegisterParams = mips->getParamRegisters().size();

    bool regMapped = false;
    if (paramPos < maxRegisterParams){
        regMapped = true;
    }

    if (regMapped)
    {
        //copy from reg to local stack
        addVar(var); // asign variable a stack place
        const int localOffset = getVarOffset(var);
        const mips::Register *src = mips->getParamRegisters()[paramPos]; // first param has position 0
        EntryCode << Indent << "  #Variable " << var.getName() << " is transfered through register" << "\n";
        EntryCode << Indent << "sw " +  src->getAsmName() << ", " << -localOffset << "($fp)"  << "\n";
        instrSize += 4;
    } else {
        int fpOffBytes = 4 + 4 * ((paramPos+1)-maxRegisterParams);
        varToStackTable[&var] = -fpOffBytes; // negative because stack is top-down
        EntryCode << Indent << "  #Variable " << var.getName() << " got stack posiotion " <<fpOffBytes << "($fp)\n";
        /*
        // copy from stack to local stack using PARAMregister 0
        const mips::Register *src = mips->getParamRegisters()[0];

        int fpOffBytes = 4 + 4 * ((paramPos+1)-maxRegisterParams); // skip the RA+FP -> first is at 8

        // load and store
        EntryCode << Indent << "  #Variable " << var.getName() << " is transfered through stack" << "\n";
        EntryCode << Indent << "lw " << src->getAsmName() << ", " << fpOffBytes   << "($fp)" << "\n";
        EntryCode << Indent << "sw " << src->getAsmName() << ", " << -localOffset << "($fp)" << "\n";
        */
    }
}

const ir::Function *FunctionContext::getFunction() const
{
    return func;
}

int FunctionContext::getVarOffset(ir::NamedValue &var)
{
    addVar(var);
    //if ( varToStackTable.find(&var) == varToStackTable.end() ) {
    //  return -1;
    //} else {
      return varToStackTable.at(&var);
    //}
}

const std::stringstream FunctionContext::getInstructions()
{
    std::stringstream instr, epilog;

    int localInstrCounter = 0;

    instr << func->getName() + " :      #FUNCTION LABEL\n";              // label
    instr << mips->getFunctionPrologue();
    localInstrCounter += 16;

    instr << Indent << "#local vars: "<< varToStackTable.size() << "\n";
    instr << Indent << "#spilled vars: "<< spillTable.size() << "\n";
    instr << Indent << "#Make place for local + spilled \n";
    instr << Indent << "addi $sp, $sp, " << -(stackCounter-4) << " \n" ;
    localInstrCounter += 4;
    instr << EntryCode.rdbuf()->str();

    epilog << func->getName() + "_$return:\n";
    epilog << Indent << "#restore callee saved registers\n";

    // callee saved registers SAVING place
    instr << Indent <<  "#callee saved registers\n";
    instr << Indent <<  "addi $sp, $sp, " << -(int)calleeSavedSet.size()*4 << " \n" ;
    localInstrCounter += 4;
    int offset = 0;
    for ( auto & item : calleeSavedSet)
    {
        instr << Indent <<  "sw " + item->getAsmName()  << ", " <<  -(int)(offset+stackCounter) <<"($fp)" << " \n" ;
        epilog << Indent << "lw " + item->getAsmName()  << ", " <<  -(int)(offset+stackCounter) <<"($fp)" << " \n" ;
        offset += 4;
        localInstrCounter += 8;
    }


    for(auto const &it : blockContextTable){
        instr <<  it.getInstructions();
    }

    instr << epilog.str() << mips->getFunctionEpilogue();
    localInstrCounter += 20;

    instrSize += localInstrCounter;

    return instr;
}

const mips::MIPS *FunctionContext::getMips() const
{
    return mips;
}

void FunctionContext::testCalleeSaved(const mips::Register *reg)
{
    for (auto &it : mips->getCalleeSavedRegisters())
    {
        if (it == reg)
            calleeSavedSet.insert(reg);
    }
}

unsigned int backend::FunctionContext::getSpillTableFreePos()
{
    for(auto &it: spillTable){
        if (it.val == nullptr)
            return (&it - &spillTable[0]);
    }
    spillItem item;
    item.val = nullptr;
    item.offset = stackCounter;
    stackCounter += 4;
    spillTable.push_back(item);
    return spillTable.size()-1;
}

unsigned int FunctionContext::spillTemp(ir::Value *val)
{
    unsigned offset = getSpillTableFreePos();
    spillTable[offset].val = val;
    return spillTable[offset].offset;
}

int FunctionContext::unspillTemp(ir::Value *val)
{
    for(auto &it : spillTable){
        if (val == it.val){
            it.val = nullptr;
            return it.offset;
        }
    }
    return -1;

}

void FunctionContext::cleanspillTable()
{
    for(auto &it : spillTable)
        it.val = nullptr;
}

ConstStringData &FunctionContext::getStringTable()
{
    return parent->getStringTable();
}

void FunctionContext::addInstrSize(int size)
{
    instrSize += size;
}

unsigned FunctionContext::getInstrSize()
{
    return instrSize;
}


}// namespace end

