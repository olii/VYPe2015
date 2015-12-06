#include "context.h"
#include "ir/function.h"

namespace backend{


BlockContext::BlockContext()
{

}

BlockContext::BlockContext(backend::FunctionContext *parrent, ir::BasicBlock *block)
{
    this->parent = parrent;
    this->block = block;
}
void BlockContext::addCanonicalInstruction(std::string &inst)
{
    text += FunctionContext::Indentation + inst + "\n";
}

void BlockContext::addInstruction(std::string inst, std::string dst, int offset, std::string op2, int offset2, std::string op3)
{
    text += FunctionContext::Indentation + inst;
    text += " " + dst;

    if (op2.empty()) {
        text += "\n";
        return;
    }

    text += ", ";
    if (offset != 0){
        text += std::to_string(offset) + "(" + op2 + ")";
    } else {
         text += op2;
    }


    if (op3.empty()) {
        text += "\n";
        return;
    }

    text += ", ";

    if (offset2 != 0){
        text += std::to_string(offset2) + "(" + op3 + ")";
    } else {
         text += op3;
    }

    text += "\n";
    return;
}



void BlockContext::unlockVar(ir::Value *val)
{
    varToReg[val].locked = false;

    //  if returning Temporary Variable than it will be read and is no longer needed to be stored in register
    if (val->getType() == ir::Value::Type::TEMPORARY)
    {
        arch::Register *reg = varToReg[val].reg;
        varToReg.erase(val);
        RegToVar.erase(reg);
    }
}

void BlockContext::removeVictim()
{

    // try to remove temporaries
    for (auto &it :varToReg ){
        if ( it.first->getType() == ir::Value::Type::TEMPORARY && !(it.second.locked))
        {
            // got victim - erase
            arch::Register *reg = it.second.reg;
            RegToVar.erase(reg);
            varToReg.erase(it.first);
            return;
        }
    }

    // if no temporaries  remove one constant
    for (auto &it :varToReg ){
        if ( it.first->getType() == ir::Value::Type::CONSTANT && !(it.second.locked))
        {
            // got victim - erase
            arch::Register *reg = it.second.reg;
            RegToVar.erase(reg);
            varToReg.erase(it.first);
            return;
        }
    }

    // todo -- remove mapped variable


    // TODO:: better remove algo

}

arch::Register *BlockContext::getRegister(ir::Value *val, bool locked)
{

    if ( varToReg.find(val) != varToReg.end() ) {
        return varToReg[val].reg;
    }

    // there is no value in register -- alocating one

    std::vector<arch::Register*> reglist = parent->getMips()->getEvalRegisters(); // possible eval registers

    //check if we have free space
    if ( RegToVar.size() >= reglist.size()  )  {
        // no free space
        removeVictim();
    }



    for (auto &reg: reglist)
    {
        if ( RegToVar.find(reg) == RegToVar.end() ) {
            //this register is free

            //copy value if necessary
            if (val->getType() == ir::Value::Type::NAMED)
            {
                ir::NamedValue *namVal = static_cast<ir::NamedValue*>(val);
                if (parent->getVarOffset(*namVal) == -1){
                    //not yet allocated on stack
                    parent->addVar(*namVal); // reserve place on stack
                    varToReg[val] = RegFlag{ reg, false, locked};
                    RegToVar[reg] = VarFlag{ val, false };
                } else
                {
                    // already on stack
                    varToReg[val] = RegFlag{ reg, true, locked};
                    RegToVar[reg] = VarFlag{ val, true };
                    int offset = parent->getVarOffset(*namVal);

                    addInstruction("lw", reg->getIDName(), 0, std::to_string(-offset) + "($fp)");

                }
            } else if ( val->getType() == ir::Value::Type::CONSTANT )
            {

                varToReg[val] = RegFlag{ reg, true, locked };
                RegToVar[reg] = VarFlag{ val, true };

                if (val->getDataType() == ir::Value::DataType::INT)
                {
                    ir::ConstantValue<int> *conVal = static_cast<ir::ConstantValue<int>*>(val);
                    int Value = conVal->getConstantValue();

                    addInstruction("li", reg->getIDName(), 0, std::to_string(Value));

                }


            } else if ( val->getType() == ir::Value::Type::TEMPORARY )
            {
                varToReg[val] = RegFlag{ reg, false, locked };
                RegToVar[reg] = VarFlag{ val, false };
            }


            parent->calleeSaved(reg); // what if callee saved
            return reg;
        }
    }


    return nullptr;
}

std::string BlockContext::getInstructions()
{
    std::string instr;
    //label
    instr += parent->getFunction()->getName() + "_" + std::to_string(block->getId()) + " :    #BLOCK LABEL\n";

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
    //prolog += "#Variable " + var.getName() + " got offset: " + std::to_string(stackCounter) +"\n";
    stackCounter += 4; // add stack offset, 4 bytes
}

void FunctionContext::addVar(ir::NamedValue &var, unsigned paramPos, unsigned /*paramAll*/)
{
    bool regMapped = false;
    if (paramPos <= mips.getMaxRegisterParameters()){
        regMapped = true;
    }


    addVar(var);
    int localOffset = getVarOffset(var);

    if (regMapped)
    {
        //copy from reg to local stack
        arch::Register *src = mips.getParamRegisters()[paramPos-1]; // -1 because first param has position 1, not 0
        prolog +=  "  sw " +  src->getIDName() +", " + std::to_string(-localOffset) + "($fp)"  + "\n"; ;
    } else {
        // copy from stack to local stack using PARAMregister 0
        arch::Register *src = mips.getParamRegisters()[0];

        int fpOffBytes = 4 + 4 * (paramPos-mips.getMaxRegisterParameters()); // skip the RA+FP -> first is at 8

        // load and store
        prolog +=  "  lw " + src->getIDName() + ", " + std::to_string(fpOffBytes)  +"($fp)" + "\n";
        prolog +=  "  sw " +  src->getIDName() +", " + std::to_string(-localOffset) + "($fp)" + "\n";

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
    std::string instr, epilog;

    instr += func->getName() + " :      #FUNCTION LABEL\n";              // label
    instr += mips.getFunctionPrologue();

    instr += "  #parameter copy + place for local vars\n";
    instr += "  addi $sp, $sp, " + std::to_string(-(int)(varStackMap.size())*4) + " \n" ;
    instr += prolog;

    // callee saved registers SAVING place
    instr += "  #callee saved registers\n";
    instr += "  addi $sp, $sp, " + std::to_string(-(int)calleeSavedSet.size()*4) + " \n" ;
    int offset = 0;
    for ( auto & item : calleeSavedSet)
    {
        instr += "  sw " + item->getIDName()  + ", " +  std::to_string(-(int)(offset+stackCounter)) +"($fp)" + " \n" ;
        offset += 4;
    }


    for(auto &it : blocks){
        instr +=  it.second.getInstructions();
    }

    instr = instr + epilog + mips.getFunctionEpilogue();

    return instr;
}

arch::MIPS *FunctionContext::getMips()
{
    return &mips;
}

void FunctionContext::calleeSaved(arch::Register *reg)
{
    for (auto &it : mips.getCalleeSavedRegisters())
    {
        if (it == reg)
            calleeSavedSet.insert(reg);
    }
}


}

