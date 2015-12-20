#include <algorithm>
#include <string>

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
    updateInstrSize(inst);
    text << backend::Indent;
    text << inst << " " << reg.getAsmName() << "\n";;
}

void BlockContext::addInstruction(const std::string &inst, const std::string &op)
{
    updateInstrSize(inst);
    text << backend::Indent;
    text << inst << " " << op << "\n";;
}

void BlockContext::addInstruction(const std::string &inst, const mips::Register &dst, const mips::Register &src)
{
    updateInstrSize(inst);
    text << backend::Indent;
    text << inst << " " << dst.getAsmName() << ", " << src.getAsmName() << "\n";
}

void BlockContext::addInstruction(const std::string &inst, const mips::Register &dst, const int imm)
{
    updateInstrSize(inst);
    text << backend::Indent;
    text << inst << " " << dst.getAsmName() << ", " << imm << "\n";
}

void BlockContext::addInstruction(const std::string &inst, const mips::Register &reg, const std::string &label)
{
    updateInstrSize(inst);
    text << backend::Indent;
    text << inst << " " << reg.getAsmName() << ", " << label << "\n";
}

void BlockContext::addInstruction(const std::string &inst, const ir::BasicBlock *block)
{
    updateInstrSize(inst);
    const BlockContext* context = parent->getBlockContext(block);

    text << backend::Indent;
    text << inst << " " << context->getName() << "\n";
}

void BlockContext::addInstruction(const std::string &inst, const ir::Function *func)
{
    updateInstrSize(inst);
    text << backend::Indent;
    text << inst << " " << func->getName() << "\n";
}

void BlockContext::addInstruction(const std::string &inst, const mips::Register &dst,const int imm, const mips::Register &src)
{
    updateInstrSize(inst);
    text << backend::Indent;
    text << inst << " " << dst.getAsmName() << ", " << imm << "(" << src.getAsmName() << ")" << "\n";
}

void BlockContext::addInstruction(const std::string &inst, const mips::Register &dst, const mips::Register &op1, const int imm)
{
    updateInstrSize(inst);
    text << backend::Indent;
    text << inst << " " << dst.getAsmName() << ", "  << op1.getAsmName() << ", " << imm << "\n";
}

void BlockContext::addInstruction(const std::string &inst, const mips::Register &op1, const mips::Register &op2, const ir::BasicBlock *block)
{
    updateInstrSize(inst);
    const BlockContext* context = parent->getBlockContext(block);

    text << backend::Indent;
    text << inst << " " << op1.getAsmName() << ", "  << op2.getAsmName() << ", " << context->getName() << "\n";
}

void BlockContext::addInstruction(const std::string &inst, const mips::Register &dst, const mips::Register &op1, const mips::Register &op2)
{
    updateInstrSize(inst);
    text << backend::Indent;
    text << inst << " " << dst.getAsmName() << ", "  << op1.getAsmName() << ", "  << op2.getAsmName() << "\n";
}

void BlockContext::addInstruction(const std::string &inst, const mips::Register &dst, const mips::Register &op1, const int imm, const mips::Register &op2)
{
    updateInstrSize(inst);
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

    // try to find item in registers
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
                } else if (val->getDataType() == ir::Value::DataType::STRING){
                    const ir::ConstantValue<std::string> *mapped = static_cast<const ir::ConstantValue<std::string>*>(it.val);
                    const ir::ConstantValue<std::string> *requested = static_cast<const ir::ConstantValue<std::string>*>(val);
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

        } else if (val->getDataType() == ir::Value::DataType::STRING){
            //ir::ConstantValue<std::string> *tmp = static_cast<ir::ConstantValue<std::string>*>(val);
            addInstruction("la", *(item->reg), getStringTable().getLabel(val));
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

ConstStringData &backend::BlockContext::getStringTable()
{
    return parent->getStringTable();
}

void BlockContext::updateInstrSize(const std::string &str)
{

    std::string lower;
    transform(str.begin(), str.end(), lower.begin(),::tolower);

    if (lower == "la"){
        parent->addInstrSize(8);
    } else if (lower == "li"){
        parent->addInstrSize(8);
    } else if (lower == "bgt"){
        parent->addInstrSize(8);
    } else if (lower == "blt"){
        parent->addInstrSize(8);
    } else if (lower == "bge"){
        parent->addInstrSize(8);
    } else if (lower == "ble"){
        parent->addInstrSize(8);
    } else if (lower == "blez"){
        parent->addInstrSize(8);
    } else if (lower == "bgtu"){
        parent->addInstrSize(8);
    } else if (lower == "bgtz"){
        parent->addInstrSize(8);
    } else if (lower == "mul"){
        parent->addInstrSize(8);
    } else if (lower == "div"){
        parent->addInstrSize(8);
    } else if (lower == "rem"){
        parent->addInstrSize(8);
    } else{
         parent->addInstrSize(4);
    }
}



} // namespace end

