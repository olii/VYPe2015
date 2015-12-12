#include <algorithm>

#include "blockcontext.h"
#include "backend/functioncontext.h"
#include "ir/function.h"



namespace backend{

BlockContext::BlockContext(backend::FunctionContext *Parent, const ir::BasicBlock *Block):
    parent(Parent), block(Block)
{
    registerTable.resize(getMips()->getEvalRegisters().size());
    auto tableItem = registerTable.begin();

    for(auto &regIter: getMips()->getEvalRegisters()){
        tableItem->reg = regIter;
        tableItem->val = nullptr;
        tableItem++;
    }
}


BlockContext::~BlockContext()
{

}

const std::string BlockContext::getName() const
{
    return parent->getFunction()->getName() + "_$" + std::to_string(block->getId());
}

const std::string BlockContext::getInstructions() const
{
    std::string instr;

    instr += getName() + ":    #BLOCK START LABEL\n";
    //instr += Indent + "addi $sp, $sp, " + std::to_string(-(int)spillTable.size() * 4) + "\n";
    instr += text.str();
    //instr += Indent + "addi $sp, $sp, " + std::to_string(spillTable.size() * 4) + "\n";

    return instr;
}

const mips::MIPS *BlockContext::getMips() const
{
    return parent->getMips();
}

const ir::BasicBlock *BlockContext::getBlock() const
{
    return block;
}

void BlockContext::addCanonicalInstruction(const std::string &inst)
{
    text << backend::Indent << inst << "\n";
}

void BlockContext::addLabel(const std::string &label)
{
    text << label << ":\n";
}

void BlockContext::addInstruction(const std::string &inst, const mips::Register &reg)
{
    text << backend::Indent;
    text << inst << " " << reg.getAsmName() << "\n";;
}

void BlockContext::addInstruction(const std::string &inst, const mips::Register &dst, const mips::Register &src)
{
    text << backend::Indent;
    text << inst << " " << dst.getAsmName() << ", " << src.getAsmName() << "\n";
}

void BlockContext::addInstruction(const std::string &inst, const mips::Register &dst, const int imm)
{
    text << backend::Indent;
    text << inst << " " << dst.getAsmName() << ", " << imm << "\n";
}

void BlockContext::addInstruction(const std::string &inst, const ir::BasicBlock *block)
{
    const BlockContext* context = parent->getBlockContext(block);

    text << backend::Indent;
    text << inst << " " << context->getName() << "\n";
}

void BlockContext::addInstruction(const std::string &inst, const ir::Function *func)
{
    text << backend::Indent;
    text << inst << " " << func->getName() << "\n";
}

void BlockContext::addInstruction(const std::string &inst, const mips::Register &dst,const int imm, const mips::Register &src)
{
    text << backend::Indent;
    text << inst << " " << dst.getAsmName() << ", " << imm << "(" << src.getAsmName() << ")" << "\n";
}

void BlockContext::addInstruction(const std::string &inst, const mips::Register &dst, const mips::Register &op1, const int imm)
{
    text << backend::Indent;
    text << inst << " " << dst.getAsmName() << ", "  << op1.getAsmName() << ", " << imm << "\n";
}

void BlockContext::addInstruction(const std::string &inst, const mips::Register &op1, const mips::Register &op2, const ir::BasicBlock *block)
{
    const BlockContext* context = parent->getBlockContext(block);

    text << backend::Indent;
    text << inst << " " << op1.getAsmName() << ", "  << op2.getAsmName() << ", " << context->getName() << "\n";
}

void BlockContext::addInstruction(const std::string &inst, const mips::Register &dst, const mips::Register &op1, const mips::Register &op2)
{
    text << backend::Indent;
    text << inst << " " << dst.getAsmName() << ", "  << op1.getAsmName() << ", "  << op2.getAsmName() << "\n";
}

void BlockContext::addInstruction(const std::string &inst, const mips::Register &dst, const mips::Register &op1, const int imm, const mips::Register &op2)
{
    text << backend::Indent;
    text << inst << " " << dst.getAsmName() << ", "  << op1.getAsmName() << ", " << imm << "(" << op2.getAsmName() << ")" << "\n";
}


void BlockContext::saveUnsavedVariables()
{
    for( auto &it: registerTable){
        if(it.val == nullptr) continue;
        if(it.val->getType() == ir::Value::Type::NAMED && it.saved == false)
        {
            ir::NamedValue *value = static_cast<ir::NamedValue*>(it.val);
            int offset = parent->getVarOffset(*value);
            addInstruction("sw", *(it.reg),-offset, getMips()->getFramePointer());
            it.saved = true;
        }
    }
}

void BlockContext::saveTemporaries()
{
    for( auto &it: registerTable){
        if(it.val == nullptr) continue;
        if(it.val->getType() == ir::Value::Type::TEMPORARY)
        {
            unsigned int pos =parent->spillTemp(it.val);

            ir::TemporaryValue* tmp = static_cast<ir::TemporaryValue*>(it.val);
            addCanonicalInstruction("# temporaryValue " + tmp->getSymbolicName() + " got spill offset ... " + std::to_string(-(int)pos) + "($fp)");
            addInstruction("sw", *(it.reg), -pos, getMips()->getFramePointer());

            it.val = nullptr;             // remove from mapping
        }
    }
}

void BlockContext::clearCallerRegisters()
{
    for( auto &it: registerTable){
        auto &vector = getMips()->getCallerSavedRegisters();

        if ( std::find(vector.begin(), vector.end(), it.reg) != vector.end() )
        {
            it.val = nullptr;
        }
    }
}

const mips::Register *BlockContext::getRegister(ir::Value *val, bool load)
{
    // if 0 return register $0
    if (val->getType() == ir::Value::Type::CONSTANT )
    {
        if ( ( (val->getDataType()) == ir::Value::DataType::INT ) ){
            const ir::ConstantValue<int> *tmp = static_cast<const ir::ConstantValue<int>*>(val);
            if (tmp->getConstantValue() == 0)
                return parent->getMips()->getZero();
        } else if ( ( (val->getDataType()) == ir::Value::DataType::CHAR ) ){
            const ir::ConstantValue<char> *tmp = static_cast<const ir::ConstantValue<char>*>(val);
            if (tmp->getConstantValue() == 0)
                return parent->getMips()->getZero();
        }
    }

    for( auto &it: registerTable){
        if (it.val == nullptr) continue;

        if (it.val == val){ // match
            it.lruValue = 0;

            if (val->getType() == ir::Value::Type::TEMPORARY) // remove temporary val form mapping
                it.val = nullptr;

            return it.reg;
        }
        else if ((val->getType() == ir::Value::Type::CONSTANT) && (it.val->getType() ==  ir::Value::Type::CONSTANT)){
            if (val->getDataType() == it.val->getDataType())
            {
                if (val->getDataType() == ir::Value::DataType::INT){
                    const ir::ConstantValue<int> *mapped = static_cast<const ir::ConstantValue<int>*>(it.val);
                    const ir::ConstantValue<int> *requested = static_cast<const ir::ConstantValue<int>*>(val);
                    if (mapped->getConstantValue() == requested->getConstantValue())
                    {
                        it.lruValue = 0;
                        return it.reg;
                    }
                } else if (val->getDataType() == ir::Value::DataType::CHAR){
                    const ir::ConstantValue<char> *mapped = static_cast<const ir::ConstantValue<char>*>(it.val);
                    const ir::ConstantValue<char> *requested = static_cast<const ir::ConstantValue<char>*>(val);
                    if (mapped->getConstantValue() == requested->getConstantValue())
                    {
                        it.lruValue = 0;
                        return it.reg;
                    }
                }
            }
        }
    }

    // there is no value in register -- alocating one
    registerItem *item = getFreeTableItem();

    item->lruValue = 0; // set as last used
    item->saved = false;
    item->val = val;
    parent->testCalleeSaved(item->reg);

    // emmit instructions

    if (val->getType() == ir::Value::Type::CONSTANT){
        if (val->getDataType() == ir::Value::DataType::INT){
            ir::ConstantValue<int> *tmp = static_cast<ir::ConstantValue<int>*>(val);
            addInstruction("li", *(item->reg), tmp->getConstantValue());
        } else if (val->getDataType() == ir::Value::DataType::CHAR){
            ir::ConstantValue<char> *tmp = static_cast<ir::ConstantValue<char>*>(val);
            addInstruction("li", *(item->reg), tmp->getConstantValue());
        }


    } else if (val->getType() == ir::Value::Type::NAMED){
        ir::NamedValue *namVal = static_cast<ir::NamedValue*>(val);

        addCanonicalInstruction("#variable " + namVal->getName() + " got register "+ item->reg->getAsmName() );

        item->saved = true;
        int offset = parent->getVarOffset(*namVal);
        if (load){
            // load to register if required
            addInstruction("lw", *(item->reg), -offset, getMips()->getFramePointer());
        }

    } else if (val->getType() == ir::Value::Type::TEMPORARY){
        ir::TemporaryValue *tempVal = static_cast<ir::TemporaryValue*>(val);
        addCanonicalInstruction("#temporary " + tempVal->getSymbolicName() + " got register "+ item->reg->getAsmName() );
        if (load){
            // check if has been spilled, if yes, load it to register
            int offset = parent->unspillTemp(val);
            if (offset != -1){
                // spilled item, load it back to register
                    addInstruction("lw", *(item->reg),-offset, getMips()->getFramePointer());
            }
        }
    }

    return item->reg;

}

const mips::Register *BlockContext::getFreeRegister()
{
    registerItem *item = getFreeTableItem();
    return item->reg;
}

void BlockContext::markChanged(const mips::Register *reg)
{
    for( auto &it: registerTable){
        if (it.reg == reg){
            it.saved = false;
            return;
        }
    }
}

void BlockContext::updateLRU()
{
    for( auto &it: registerTable)
        it.lruValue += 1;
}

registerItem *BlockContext::getFreeTableItem()
{
    for( auto &it: registerTable){
        if (it.val == nullptr){
            return &it;
        }
    }
    removeVictim();

    return getFreeTableItem();
}

void BlockContext::removeVictim()
{
    unsigned int maxLRU = 0;
    int posMaxLRU = 0;

    for( auto &it: registerTable){
        if (it.val == nullptr) return;

        if (it.lruValue > maxLRU){
            maxLRU = it.lruValue;
            posMaxLRU = &it - &registerTable[0];
        }
    }

    addCanonicalInstruction("#Spilling register " + registerTable[posMaxLRU].reg->getAsmName() + " with LRU = " + std::to_string(maxLRU));
    ir::Value* val = registerTable[posMaxLRU].val;



    if (val->getType() == ir::Value::Type::CONSTANT)
    {
        addCanonicalInstruction("#Spilled variable is constant -> removed ");

    } else if (val->getType() == ir::Value::Type::NAMED){
        ir::NamedValue *named = static_cast<ir::NamedValue *>(val);
        addCanonicalInstruction("#Spilled variable is " + named->getName());
        if (!registerTable[posMaxLRU].saved){
            int offset = parent->getVarOffset(*named);
            addInstruction("sw", *(registerTable[posMaxLRU].reg),-offset, getMips()->getFramePointer());
        }

    } else if (val->getType() == ir::Value::Type::TEMPORARY){
        ir::TemporaryValue* tmp = static_cast<ir::TemporaryValue*>(val);
        addCanonicalInstruction("#Spilled temporary is " + tmp->getSymbolicName());

        unsigned int offset = parent->spillTemp(val);

        addCanonicalInstruction("# temporaryValue " + tmp->getSymbolicName() + " got spill offset ... " + std::to_string(-(int)offset) + "($fp)");
        addInstruction("sw", *(registerTable[posMaxLRU].reg), -offset, getMips()->getFramePointer());
    }


    registerTable[posMaxLRU].val = nullptr;

}



/*

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

    // registers are full of NamedValue or locked vars
    // try to remove first saved variable from register
    for (auto &it :RegToVar ){
        if ( it.second.saved && !(varToReg[it.second.val].locked))
        {
            // got victim - erase
            varToReg.erase(it.second.val);
            RegToVar.erase(it.first);
            return;
        }
    }

    // remove mapped variable UNSAVED
    // MONTECARLO REMOVER

    while( 1 ){
        std::random_device rd; // obtain a random number from hardware
        std::mt19937 eng(rd()); // seed the generator
        std::uniform_int_distribution<> distr(0, RegToVar.size()-1); // define the range
        int IndexOfRemoval = distr(eng);

        arch::Register *rem = parent->getMips()->getEvalRegisters()[IndexOfRemoval];

        if (varToReg[RegToVar[rem].val].locked || RegToVar[rem].val->getType() != ir::Value::Type::NAMED) {
            continue; // cannot remove because locked
        }
        // do remove var:

        //1. save to place
        ir::NamedValue *value = static_cast<ir::NamedValue *>(RegToVar[rem].val);
        int offset = parent->getVarOffset(*value);
        addInstruction("sw", rem->getIDName(), -offset, "$fp");
        //2. remove from mapping

        varToReg.erase(RegToVar[rem].val);
        RegToVar.erase(rem);
        return;
    }



    // TODO:: better remove algo

}

std::vector<std::pair<ir::Value* , int >> BlockContext::saveCallerRegisters()
{
    std::vector<std::pair<ir::Value* , int >> map;


    int tempCount = 0;
    std::string loader;

    for(auto item: parent->getMips()->getCallerSavedRegisters()){
        if ( RegToVar.find(item) == RegToVar.end() )
            continue;

        if ( RegToVar[item].val->getType() == ir::Value::Type::NAMED &&
             RegToVar[item].saved == false)
        {
            ir::NamedValue *value = static_cast<ir::NamedValue*>(RegToVar[item].val);
            int offset = parent->getVarOffset(*value);
            addInstruction("sw", item->getIDName(), -offset, "$fp");
            RegToVar[item].saved = true;
        }
        if ( RegToVar[item].val->getType() == ir::Value::Type::TEMPORARY &&
             varToReg[RegToVar[item].val].locked == true)
        {
            ir::Value *value = (RegToVar[item].val);
            std::pair<ir::Value* , int > pair(value, tempCount*4);
            map.push_back(std::move(pair));

            if (!loader.empty())
            loader += "\n";
            loader += FunctionContext::Indentation + "sw " +  varToReg[value].reg->getIDName() + ", "+ std::to_string(tempCount*4) + "($sp)";
            tempCount++;
        }

    }
    if (!loader.empty())
    {
        addCanonicalInstruction("#preserve stack");
        addInstruction("addi", "$sp", 0, "$sp", 0, std::to_string(-tempCount*4));
        addCanonicalInstruction(loader);
        addCanonicalInstruction("#preserve stack end");
    }

    return map;
}





arch::Register *BlockContext::getRegister(ir::Value *val, bool locked, bool implicitLoad)
{
    // if int(0) return $0
    if (val->getType() == ir::Value::Type::CONSTANT &&  (val->getDataType()) == ir::Value::DataType::INT)
    {
        const ir::ConstantValue<int> *tmp = static_cast<const ir::ConstantValue<int>*>(val);
        if (tmp->getConstantValue() == 0)
            return parent->getMips()->getZero();
    }


    if ( varToReg.find(val) != varToReg.end() ) {
        if (val->getType() == ir::Value::Type::TEMPORARY){
            varToReg[val].locked = false;
        }

        return varToReg[val].reg;
    }

    // there is no value in register -- alocating one

    const std::vector<arch::Register*> &reglist = parent->getMips()->getEvalRegisters(); // possible eval registers

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
                    varToReg[val] = RegFlag{ reg, true, locked};
                    RegToVar[reg] = VarFlag{ val, true };
                } else
                {
                    // already on stack
                    varToReg[val] = RegFlag{ reg, true, locked};
                    RegToVar[reg] = VarFlag{ val, true };
                    int offset = parent->getVarOffset(*namVal);

                    if (implicitLoad){
                        // load to register if required
                        addInstruction("lw", reg->getIDName(), 0, std::to_string(-offset) + "($fp)");
                    }

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


            parent->testCalleeSaved(reg); // what if callee saved
            return reg;
        }
    }


    return nullptr;
}

arch::Register *BlockContext::getFreeRegister()
{
    // there is no value in register -- alocating one
    const std::vector<arch::Register*> &reglist = parent->getMips()->getEvalRegisters(); // possible eval registers

    //check if we have free space
    if ( RegToVar.size() >= reglist.size()  )  {
        // no free space
        removeVictim();
    }

    for (auto &reg: reglist)
    {
        if ( RegToVar.find(reg) == RegToVar.end() ) {
            //this register is free
            return reg;
        }
    }


    return nullptr;
}


*/

} // namespace end

