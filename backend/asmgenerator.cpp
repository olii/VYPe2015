#include "asmgenerator.h"
#include <iostream>
#include <utility>


namespace backend {
ASMgenerator::ASMgenerator()
{

    std::cout << "<< TODO: temporaries spilling as function context, instructions, asm loader>>" << std::endl;
}

ASMgenerator::~ASMgenerator()
{

}

void ASMgenerator::translateIR(ir::Builder &builder){
    for (auto& it : builder.getFunctions())
        it.second->accept(*this);
}

void ASMgenerator::visit(ir::Function *func)
{
    std::cout << "#<< Entery point of function " << func->getName() << " >>" << std::endl;


    const std::vector<ir::Value*> parameters = func->getParameters();

    //create new context
    FunctionContext fc(func, &mips);
    //store it by value
    context.emplace(func, std::move(fc));
    // set pointer to newly added reference

    activeFunction = &context.at(func);

    for (unsigned int i = 0; i<parameters.size(); i++)
    {
        ir::NamedValue* param = static_cast<ir::NamedValue*>(parameters[i]);
        activeFunction->addVar(*param, i);
    }

    // load basic blocks
    for (ir::BasicBlock* bb : func->getBasicBlocks())
    {
        activeFunction->addBlock(bb);
    }

    // iterate over each basicblock
    for (ir::BasicBlock* bb : func->getBasicBlocks())
    {
        bb->accept(*this);
    }

    std::cout << activeFunction->getInstructions().str() << std::endl ;
}

void ASMgenerator::visit(ir::BasicBlock *block)
{
    activeFunction->addBlock(block);
    activeFunction->setActiveBlock(block);
    for (ir::Instruction* inst : block->getInstructions()){
            inst->accept(*this);
            activeFunction->Active()->updateLRU();
    }
    activeFunction->cleanspillTable();

}


void ASMgenerator::visit(ir::NamedValue *value)
{/*
    activeFunction->Active()->getRegister(value);
*/}

void ASMgenerator::visit(ir::TemporaryValue *value)
{/*
    activeFunction->Active()->getRegister(value);
*/}

void ASMgenerator::visit(ir::ConstantValue<int> *value)
{/*

*/}

void ASMgenerator::visit(ir::ConstantValue<char> *value)
{/*

*/}

void ASMgenerator::visit(ir::ConstantValue<std::string> *value)
{/*

*/}

void ASMgenerator::visit(ir::AssignInstruction *instr)
{
    ir::Value *dest = instr->getResult();
    ir::Value *operand = instr->getOperand();

    const mips::Register *operandReg = activeFunction->Active()->getRegister(operand);
    const mips::Register *destReg = activeFunction->Active()->getRegister(dest, false);
    activeFunction->Active()->markChanged(destReg);

    activeFunction->Active()->addInstruction("move", *destReg, *operandReg);


}

void ASMgenerator::visit(ir::DeclarationInstruction *instr)
{
    ir::Value *operand = instr->getOperand();
    activeFunction->addVar(*(static_cast<ir::NamedValue*>(operand)));
    activeFunction->Active()->getRegister(operand, false);
}

void ASMgenerator::visit(ir::JumpInstruction *instr)
{
    activeFunction->Active()->saveUnsavedVariables();
    ir::BasicBlock *jumpBlock = instr->getFollowingBasicBlock();
    activeFunction->Active()->addInstruction("j", jumpBlock);



}

void ASMgenerator::visit(ir::CondJumpInstruction *instr)
{
    activeFunction->Active()->saveUnsavedVariables();

    ir::Value *cond =  instr->getCondition();
    ir::BasicBlock *trueJump = instr->getTrueBasicBlock();
    ir::BasicBlock *falseJump = instr->getFalseBasicBlock();

    const mips::Register *condReg = activeFunction->Active()->getRegister(cond);

    activeFunction->Active()->addInstruction("bne", *condReg, *mips.getZero(), trueJump);
    activeFunction->Active()->addInstruction("b ", falseJump);
}

void ASMgenerator::visit(ir::ReturnInstruction *instr)
{
    ir::Value *retVal = instr->getOperand();
    const mips::Register *retReg = activeFunction->Active()->getRegister(retVal);

    activeFunction->Active()->saveUnsavedVariables();
    activeFunction->Active()->addInstruction("move", *(mips.getRetRegister()), *retReg );
    activeFunction->Active()->addCanonicalInstruction("j " + activeFunction->getFunction()->getName() + "_$return\n");
}

void ASMgenerator::visit(ir::CallInstruction *instr)
{

    activeFunction->Active()->addCanonicalInstruction("#function call");
    //save all registers with namedValue
    activeFunction->Active()->saveUnsavedVariables();

    bool hasStackTransfer = instr->getArguments().size() > mips.getParamRegisters().size();
    if (hasStackTransfer)
    {
        // some parameters are stack transfered -> prepare place
        int requiredSize = (instr->getArguments().size() - mips.getParamRegisters().size()) *4;
        activeFunction->Active()->addInstruction("addi", mips.getStackPointer(), mips.getStackPointer(), -requiredSize);
    }

    int stackOffset = 0;
    unsigned int i = 1;
    for(auto &param : instr->getArguments()){
        const mips::Register *paramReg = activeFunction->Active()->getRegister(param);

        if (i<=mips.getParamRegisters().size()) {
            const mips::Register *destReg = mips.getParamRegisters()[i-1];

            activeFunction->Active()->addInstruction("move", *destReg, *paramReg);
        }  else {
            // cdecl convention
            activeFunction->Active()->addInstruction("sw", *paramReg, stackOffset, mips.getStackPointer());
            stackOffset += 4;
        }
        i++;

    }

    activeFunction->Active()->saveTemporaries();
    activeFunction->Active()->clearCallerRegisters();

    // function call
    activeFunction->Active()->addInstruction("jal", instr->getFunction());

    // return stack to valid state
    if (hasStackTransfer)
    {
        int requiredSize = (instr->getArguments().size() - mips.getParamRegisters().size()) *4;
        activeFunction->Active()->addInstruction("addi", mips.getStackPointer(), mips.getStackPointer(), requiredSize);
    }

    if (instr->getResult())
    {
        // save result if has one
        const mips::Register *destReg = activeFunction->Active()->getRegister(instr->getResult(),false);
        activeFunction->Active()->markChanged(destReg);

        activeFunction->Active()->addInstruction("move", *destReg, *(mips.getRetRegister()));
    }

}

void ASMgenerator::visit(ir::BuiltinCallInstruction *instr)
{
    const mips::Register *destReg = nullptr;

    const std::string &name = instr->getFunctionName();
    if (name.compare("print") == 0){
        builtin_print(instr->getArguments());
    } else if (name.compare("read_char") == 0){
        destReg = activeFunction->Active()->getRegister(instr->getResult(),false);
        activeFunction->Active()->markChanged(destReg);
        activeFunction->Active()->addInstruction("READ_CHAR", *destReg);
    } else if (name.compare("read_int") == 0){
        destReg = activeFunction->Active()->getRegister(instr->getResult(),false);
        activeFunction->Active()->markChanged(destReg);
        activeFunction->Active()->addInstruction("READ_INT", *destReg);
    } else if (name.compare("read_string") == 0){

    } else if (name.compare("get_at") == 0){

    } else if (name.compare("set_at") == 0){

    } else if (name.compare("strcat") == 0){

    }
}

void ASMgenerator::visit(ir::AddInstruction *instr)
{

    ir::Value *left = instr->getLeftOperand();
    ir::Value *right = instr->getRightOperand();
    ir::Value *dest = instr->getResult();

    const mips::Register *leftReg = activeFunction->Active()->getRegister(left);
    const mips::Register *rightReg = activeFunction->Active()->getRegister(right);


    const mips::Register *destReg = activeFunction->Active()->getRegister(dest, false);
    activeFunction->Active()->markChanged(destReg);

    activeFunction->Active()->addInstruction("ADD", *destReg, *leftReg, *rightReg );

}

void ASMgenerator::visit(ir::SubtractInstruction *instr)
{
    ir::Value *left = instr->getLeftOperand();
    ir::Value *right = instr->getRightOperand();
    ir::Value *dest = instr->getResult();

    const mips::Register *leftReg = activeFunction->Active()->getRegister(left);
    const mips::Register *rightReg = activeFunction->Active()->getRegister(right);


    const mips::Register *destReg = activeFunction->Active()->getRegister(dest, false);
    activeFunction->Active()->markChanged(destReg);

    activeFunction->Active()->addInstruction("SUB", *destReg, *leftReg, *rightReg );
}

void ASMgenerator::visit(ir::MultiplyInstruction *instr)
{
    ir::Value *left = instr->getLeftOperand();
    ir::Value *right = instr->getRightOperand();
    ir::Value *dest = instr->getResult();

    const mips::Register *leftReg = activeFunction->Active()->getRegister(left);
    const mips::Register *rightReg = activeFunction->Active()->getRegister(right);
    const mips::Register *destReg = activeFunction->Active()->getRegister(dest, false);
    activeFunction->Active()->markChanged(destReg);

    activeFunction->Active()->addInstruction("MUL", *destReg, *leftReg, *rightReg  );
}

void ASMgenerator::visit(ir::DivideInstruction *instr)
{
    ir::Value *left = instr->getLeftOperand();
    ir::Value *right = instr->getRightOperand();
    ir::Value *dest = instr->getResult();

    const mips::Register *leftReg = activeFunction->Active()->getRegister(left);
    const mips::Register *rightReg = activeFunction->Active()->getRegister(right);
    const mips::Register *destReg = activeFunction->Active()->getRegister(dest, false);
    activeFunction->Active()->markChanged(destReg);

    activeFunction->Active()->addInstruction("DIV", *destReg, *leftReg, *rightReg  );
}

void ASMgenerator::visit(ir::ModuloInstruction *instr)
{
    ir::Value *left = instr->getLeftOperand();
    ir::Value *right = instr->getRightOperand();
    ir::Value *dest = instr->getResult();

    const mips::Register *leftReg = activeFunction->Active()->getRegister(left);
    const mips::Register *rightReg = activeFunction->Active()->getRegister(right);
    const mips::Register *destReg = activeFunction->Active()->getRegister(dest, false);
    activeFunction->Active()->markChanged(destReg);

    activeFunction->Active()->addInstruction("DIV", *leftReg, *rightReg  );
    activeFunction->Active()->addInstruction("mfhi", *destReg );
}

void ASMgenerator::visit(ir::LessInstruction *instr)
{
    ir::Value *left = instr->getLeftOperand();
    ir::Value *right = instr->getRightOperand();
    ir::Value *dest = instr->getResult();

    const mips::Register *leftReg = activeFunction->Active()->getRegister(left);
    const mips::Register *rightReg = activeFunction->Active()->getRegister(right);
    const mips::Register *destReg = activeFunction->Active()->getRegister(dest, false);
    activeFunction->Active()->markChanged(destReg);

    activeFunction->Active()->addInstruction("SLT", *destReg,*leftReg, *rightReg);
}

void ASMgenerator::visit(ir::LessEqualInstruction *instr)
{
    ir::Value *left = instr->getLeftOperand();
    ir::Value *right = instr->getRightOperand();
    ir::Value *dest = instr->getResult();

    const mips::Register *leftReg = activeFunction->Active()->getRegister(left);
    const mips::Register *rightReg = activeFunction->Active()->getRegister(right);
    const mips::Register *destReg = activeFunction->Active()->getRegister(dest, false);
    activeFunction->Active()->markChanged(destReg);

    const mips::Register *tempReg = activeFunction->Active()->getFreeRegister();


    activeFunction->Active()->addInstruction("SLT",   *destReg, *leftReg, *rightReg );
    activeFunction->Active()->addInstruction("XOR",   *tempReg, *leftReg, *rightReg );
    activeFunction->Active()->addInstruction("SLTIU", *tempReg, *tempReg, 1);
    activeFunction->Active()->addInstruction("OR",    *destReg, *destReg, *tempReg);

}

void ASMgenerator::visit(ir::GreaterInstruction *instr)
{
    ir::Value *left = instr->getLeftOperand();
    ir::Value *right = instr->getRightOperand();
    ir::Value *dest = instr->getResult();

    const mips::Register *leftReg = activeFunction->Active()->getRegister(left);
    const mips::Register *rightReg = activeFunction->Active()->getRegister(right);
    const mips::Register *destReg = activeFunction->Active()->getRegister(dest, false);
    activeFunction->Active()->markChanged(destReg);

    activeFunction->Active()->addInstruction("SLT", *destReg,*rightReg, *leftReg);
}

void ASMgenerator::visit(ir::GreaterEqualInstruction *instr)
{
    ir::Value *left = instr->getLeftOperand();
    ir::Value *right = instr->getRightOperand();
    ir::Value *dest = instr->getResult();

    const mips::Register *leftReg = activeFunction->Active()->getRegister(left);
    const mips::Register *rightReg = activeFunction->Active()->getRegister(right);
    const mips::Register *destReg = activeFunction->Active()->getRegister(dest, false);
    activeFunction->Active()->markChanged(destReg);

    const mips::Register *tempReg = activeFunction->Active()->getFreeRegister();


    activeFunction->Active()->addInstruction("SLT",   *destReg, *rightReg, *leftReg );
    activeFunction->Active()->addInstruction("XOR",   *tempReg, *rightReg, *leftReg );
    activeFunction->Active()->addInstruction("SLTIU", *tempReg, *tempReg, 1);
    activeFunction->Active()->addInstruction("OR",    *destReg, *destReg, *tempReg);

}

void ASMgenerator::visit(ir::EqualInstruction *instr)
{

    ir::Value *left = instr->getLeftOperand();
    ir::Value *right = instr->getRightOperand();
    ir::Value *dest = instr->getResult();

    const mips::Register *leftReg = activeFunction->Active()->getRegister(left);
    const mips::Register *rightReg = activeFunction->Active()->getRegister(right);
    const mips::Register *destReg = activeFunction->Active()->getRegister(dest, false);
    activeFunction->Active()->markChanged(destReg);

    activeFunction->Active()->addInstruction("XOR",*destReg, *leftReg, *rightReg);
    activeFunction->Active()->addInstruction("SLTIU", *destReg, *destReg, 1);

}

void ASMgenerator::visit(ir::NotEqualInstruction *instr)
{
    ir::Value *left = instr->getLeftOperand();
    ir::Value *right = instr->getRightOperand();
    ir::Value *dest = instr->getResult();

    const mips::Register *leftReg = activeFunction->Active()->getRegister(left);
    const mips::Register *rightReg = activeFunction->Active()->getRegister(right);
    const mips::Register *destReg = activeFunction->Active()->getRegister(dest, false);
    activeFunction->Active()->markChanged(destReg);

    activeFunction->Active()->addInstruction("XOR",*destReg, *leftReg, *rightReg);
    activeFunction->Active()->addInstruction("SLTU", *destReg, *destReg, *mips.getZero());
}

void ASMgenerator::visit(ir::AndInstruction *instr)
{

    ir::Value *left = instr->getLeftOperand();
    ir::Value *right = instr->getRightOperand();
    ir::Value *dest = instr->getResult();

    const mips::Register *leftReg = activeFunction->Active()->getRegister(left);
    const mips::Register *rightReg = activeFunction->Active()->getRegister(right);
    const mips::Register *destReg = activeFunction->Active()->getRegister(dest, false);
    activeFunction->Active()->markChanged(destReg);

    const mips::Register *tempReg = activeFunction->Active()->getFreeRegister();

    activeFunction->Active()->addInstruction("SLTU",   *tempReg, *mips.getZero(), *leftReg );
    activeFunction->Active()->addInstruction("SLTU",   *destReg, *mips.getZero(), *rightReg );
    activeFunction->Active()->addInstruction("AND",    *destReg, *destReg, *tempReg);

}

void ASMgenerator::visit(ir::OrInstruction *instr)
{
    ir::Value *left = instr->getLeftOperand();
    ir::Value *right = instr->getRightOperand();
    ir::Value *dest = instr->getResult();

    const mips::Register *leftReg = activeFunction->Active()->getRegister(left);
    const mips::Register *rightReg = activeFunction->Active()->getRegister(right);
    const mips::Register *destReg = activeFunction->Active()->getRegister(dest, false);
    activeFunction->Active()->markChanged(destReg);

    const mips::Register *tempReg = activeFunction->Active()->getFreeRegister();

    activeFunction->Active()->addInstruction("SLTU",   *tempReg, *mips.getZero(), *leftReg );
    activeFunction->Active()->addInstruction("SLTU",   *destReg, *mips.getZero(), *rightReg );
    activeFunction->Active()->addInstruction("OR",     *destReg, *destReg, *tempReg);
}

void ASMgenerator::visit(ir::BitwiseAndInstruction *instr)
{
    ir::Value *left = instr->getLeftOperand();
    ir::Value *right = instr->getRightOperand();
    ir::Value *dest = instr->getResult();

    const mips::Register *leftReg = activeFunction->Active()->getRegister(left);
    const mips::Register *rightReg = activeFunction->Active()->getRegister(right);
    const mips::Register *destReg = activeFunction->Active()->getRegister(dest, false);
    activeFunction->Active()->markChanged(destReg);


    activeFunction->Active()->addInstruction("AND", *destReg, *leftReg, *rightReg);
}

void ASMgenerator::visit(ir::BitwiseOrInstruction *instr)
{
    ir::Value *left = instr->getLeftOperand();
    ir::Value *right = instr->getRightOperand();
    ir::Value *dest = instr->getResult();

    const mips::Register *leftReg = activeFunction->Active()->getRegister(left);
    const mips::Register *rightReg = activeFunction->Active()->getRegister(right);
    const mips::Register *destReg = activeFunction->Active()->getRegister(dest, false);
    activeFunction->Active()->markChanged(destReg);


    activeFunction->Active()->addInstruction("OR", *destReg, *leftReg, *rightReg);
}

void ASMgenerator::visit(ir::NotInstruction *instr)
{
    ir::Value *op = instr->getOperand();
    ir::Value *dest = instr->getResult();

    const mips::Register *opReg = activeFunction->Active()->getRegister(op);
    const mips::Register *destReg = activeFunction->Active()->getRegister(dest, false);
    activeFunction->Active()->markChanged(destReg);

    activeFunction->Active()->addInstruction("SLTIU", *destReg, *opReg, 1);

}

void ASMgenerator::visit(ir::TypecastInstruction *instr)
{
    ir::Value *op = instr->getOperand();
    ir::Value *dest = instr->getResult();
    const mips::Register *opReg = activeFunction->Active()->getRegister(op);
    const mips::Register *destReg = activeFunction->Active()->getRegister(dest, false);
    activeFunction->Active()->markChanged(destReg);


    activeFunction->Active()->addCanonicalInstruction("#Typecast");

    switch(op->getDataType()){
    case ir::Value::DataType::CHAR:
    {
        switch(dest->getDataType()){
            case ir::Value::DataType::INT:
            {
                //char to int - noop
                break;
            }
            case ir::Value::DataType::STRING:
            {
                //char to str -- make new string str("char");
                break;
            }
        default: break;
        }

        break;
    }
    case ir::Value::DataType::INT:
    {
        switch(dest->getDataType()){
            case ir::Value::DataType::CHAR:
            {
                //int to char -> only least significant
                activeFunction->Active()->addInstruction("MOVE", *destReg, *opReg);
                activeFunction->Active()->addInstruction("ANDI", *destReg, 0xFF);
                break;
            }
            case ir::Value::DataType::STRING:
            {
                //int to string
                break;
            }
        default: break;
        }

        break;
    }
    case ir::Value::DataType::STRING:
    {
        switch(dest->getDataType()){
            case ir::Value::DataType::CHAR:
            {
                //string to char
                break;
            }
            case ir::Value::DataType::INT:
            {
                //string to int
                break;
            }
        default: break;
        }

        break;
    }
    default: break;
    }
}

void ASMgenerator::visit(ir::BitwiseNotInstruction *instr)
{
    ir::Value *op = instr->getOperand();
    ir::Value *dest = instr->getResult();

    const mips::Register *opReg = activeFunction->Active()->getRegister(op);
    const mips::Register *destReg = activeFunction->Active()->getRegister(dest, false);
    activeFunction->Active()->markChanged(destReg);

    activeFunction->Active()->addInstruction("NOT", *destReg, *opReg);
}

void ASMgenerator::visit(ir::NegInstruction *instr)
{
    ir::Value *op = instr->getOperand();
    ir::Value *dest = instr->getResult();

    const mips::Register *opReg = activeFunction->Active()->getRegister(op);
    const mips::Register *destReg = activeFunction->Active()->getRegister(dest, false);
    activeFunction->Active()->markChanged(destReg);

    activeFunction->Active()->addInstruction("SUB", *destReg, *mips.getZero(),*opReg);
}

void ASMgenerator::builtin_print(std::vector<ir::Value *> &params)
{
    for(auto &item : params){
        const mips::Register *opReg = activeFunction->Active()->getRegister(item);

        switch (item->getDataType()) {
        case ir::Value::DataType::INT:
            activeFunction->Active()->addInstruction("print_int", *opReg);
            break;
        case ir::Value::DataType::CHAR:
            activeFunction->Active()->addInstruction("print_char", *opReg);
            break;
        case ir::Value::DataType::STRING:
            activeFunction->Active()->addInstruction("print_string", *opReg);
            break;
        default:
            break;
        }
    }
}

}


