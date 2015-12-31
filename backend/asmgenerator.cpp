/**
 * Project VYPe 2015/2016
 * Marek Milkovič (xmilko01) & Oliver Nemček (xnemce03)
 **/

#include "asmgenerator.h"
#include <iostream>
#include <utility>


namespace backend {
ASMgenerator::ASMgenerator()
{
    //std::cout << "<< TODO:instructions check, void functions >>" << std::endl;
}

ASMgenerator::~ASMgenerator()
{

}

int ASMgenerator::translateIR(ir::Builder &builder){

    for (auto& it : builder.getFunctions())
        it.second->accept(*this);
    return 0;
}

std::string ASMgenerator::getTargetCode()
{
    std::stringstream out;
    int codeSize = 0;

    out << ".text"                "\n"
           ".org 0"               "\n"
           "li $sp, 0x100000"     "\n"
           "la $gp, DATABEGIN"     "\n"
           "move $fp, $sp"        "\n"
           "jal main$0$"          "\n"
           "break"               "\n"
           "\n";
    codeSize += 7*4;
    out << "$MOVE_R2_TO_GP$:"                      "\n"
           "	move $3, $2"                        "\n"
           "	move $2, $gp"                       "\n"
           "$MOVE_R2_TO_GP_loop$:"                 "\n"
           "	lbu $4, 0($3)"                      "\n"
           "	sb $4, 0($gp)"                      "\n"
           "	addi $3, $3, 1"                     "\n"
           "	addi $gp, $gp, 1"                   "\n"
           "	bne $4,$0,$MOVE_R2_TO_GP_loop$"     "\n"
           "	jr $ra"                             "\n"
           "\n";
    codeSize += 8*4;
    out << "$STR_CMP$:"                      "\n"
           "	lb $6, 0($4)"                 "\n"
           "	lb $7, 0($5)"                 "\n"
           "	addi $4, $4, 1"               "\n"
           "	addi $5, $5, 1"               "\n"
           "	beq $6,$7,$STR_CMP_eq$"       "\n"
           "	sub $2, $6, $7"               "\n"
           "	jr $ra"                       "\n"
           "$STR_CMP_eq$:"                   "\n"
           "	BEQ $6, $0, $STR_CMP_end$"    "\n"
           "	j $STR_CMP$"                  "\n"
           "$STR_CMP_end$:"                  "\n"
           "	li $2, 0"                     "\n"
           "	jr $ra"                       "\n"
           "\n";
    codeSize += 12*4;
    out << "#Computed Code Size: " << codeSize << "\n";

    out << "\n\n\n\n\n\n\n";
    for (auto &it : context){
         out << it.second.getInstructions().str() << std::endl ;
         codeSize += it.second.getInstrSize();
         out << "#Computed Code Size: " << codeSize << "\n";
         out << "\n\n\n\n\n\n\n";
    }

    out << "\n\n\n\n\n\n\n";

    out << ".data \n";
    out << constStringData.TranslateTable().str();
    out << "\n\n\n\n\n\n\n";

    codeSize += constStringData.getDataSize();

    out << "#Computed Code Size: " << codeSize << "\n";

    if (((1024-64)*1024) < codeSize){
        throw(codeSize);
    }


    return out.str();
}

void ASMgenerator::visit(ir::Function *func)
{
    //std::cout << "#<< Entery point of function " << func->getName() << " >>" << std::endl;


    const std::vector<ir::Value*> parameters = func->getParameters();

    //create new context
    FunctionContext fc(func, this, &mips);
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

    //std::cout << activeFunction->getInstructions().str() << std::endl ;
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


void ASMgenerator::visit(ir::NamedValue* /*value*/)
{
    //activeFunction->Active()->getRegister(value);
}

void ASMgenerator::visit(ir::TemporaryValue* /*value*/)
{
    //activeFunction->Active()->getRegister(value);
}

void ASMgenerator::visit(ir::ConstantValue<int>* /*value*/)
{

}

void ASMgenerator::visit(ir::ConstantValue<char>* /*value*/)
{

}

void ASMgenerator::visit(ir::ConstantValue<std::string>* /*value*/)
{

}

void ASMgenerator::visit(ir::AssignInstruction *instr)
{
    ir::Value *dest = instr->getResult();
    ir::Value *operand = instr->getOperand();

    const mips::Register *operandReg = activeFunction->Active()->getRegister(operand);
    const mips::Register *destReg = activeFunction->Active()->getRegister(dest, false);
    activeFunction->Active()->markChanged(destReg);
    activeFunction->Active()->markUsed(operand);

    activeFunction->Active()->addInstruction("MOVE", *destReg, *operandReg);



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
    activeFunction->Active()->addInstruction("J", jumpBlock);



}

void ASMgenerator::visit(ir::CondJumpInstruction *instr)
{
    activeFunction->Active()->saveUnsavedVariables();

    ir::Value *cond =  instr->getCondition();
    ir::BasicBlock *trueJump = instr->getTrueBasicBlock();
    ir::BasicBlock *falseJump = instr->getFalseBasicBlock();

    const mips::Register *condReg = activeFunction->Active()->getRegister(cond);
    activeFunction->Active()->markUsed(cond);

    activeFunction->Active()->addInstruction("BNE", *condReg, *mips.getZero(), trueJump);
    activeFunction->Active()->addInstruction("B ", falseJump);
}

void ASMgenerator::visit(ir::ReturnInstruction *instr)
{
    activeFunction->Active()->saveUnsavedVariables();
    ir::Value *retVal = instr->getOperand();
    if (retVal != nullptr){
        const mips::Register *retReg = activeFunction->Active()->getRegister(retVal);
        activeFunction->Active()->addInstruction("MOVE", *(mips.getRetRegister()), *retReg );
        activeFunction->Active()->markUsed(retVal);
    }
    activeFunction->Active()->addInstruction("J", activeFunction->getFunction()->getName() + "_$return\n");
}

void ASMgenerator::visit(ir::CallInstruction *instr)
{
    activeFunction->Active()->addCanonicalInstruction("#function call");

    //activeFunction->Active()->saveUnsavedVariables();//save all registers with namedValue

    bool hasStackTransfer = instr->getArguments().size() > mips.getParamRegisters().size();
    int requiredSize = 4; // reserve place for GP
    if (hasStackTransfer) // some parameters are stack transfered -> prepare place
    {
        requiredSize += (instr->getArguments().size() - mips.getParamRegisters().size()) *4;
    }
    activeFunction->Active()->addInstruction("ADDI", mips.getStackPointer(), mips.getStackPointer(), -(requiredSize));

    int gpOffset = requiredSize - 4;

    activeFunction->Active()->addInstruction("SW", mips.getGlobalPointer(), gpOffset, mips.getStackPointer()); // save GP

    int stackOffset = 0;
    unsigned int i = 1;
    for(auto &param : instr->getArguments()){
        const mips::Register *paramReg = activeFunction->Active()->getRegister(param);

        if (i<=mips.getParamRegisters().size()) {
            const mips::Register *destReg = mips.getParamRegisters()[i-1];

            activeFunction->Active()->addInstruction("MOVE", *destReg, *paramReg);
        }  else {
            // cdecl convention
            activeFunction->Active()->addInstruction("SW", *paramReg, stackOffset, mips.getStackPointer());
            stackOffset += 4;
        }
        activeFunction->Active()->markUsed(param);
        i++;
    }

    /*activeFunction->Active()->saveTemporaries();
    activeFunction->Active()->clearCallerRegisters();*/


    activeFunction->Active()->saveVarsAndClear();
    //activeFunction->Active()->clearCallerRegisters();

    // function call
    activeFunction->Active()->addInstruction("JAL", instr->getFunction());

    // return stack to valid state
    activeFunction->Active()->addInstruction("LW", mips.getGlobalPointer(), gpOffset, mips.getStackPointer()); // save GP
    activeFunction->Active()->addInstruction("ADDI", mips.getStackPointer(), mips.getStackPointer(), requiredSize);




    if (instr->getResult())
    {
        // save result if has one
        const mips::Register *destReg = activeFunction->Active()->getRegister(instr->getResult(),false);
        activeFunction->Active()->markChanged(destReg);


        if (instr->getFunction()->getReturnDataType() == ir::Value::DataType::STRING){
            // copy string in R2 to local address space and move
            activeFunction->Active()->addInstruction("JAL", "$MOVE_R2_TO_GP$");
        }
            activeFunction->Active()->addInstruction("MOVE", *destReg, *(mips.getRetRegister()));

    }

}

void ASMgenerator::visit(ir::BuiltinCallInstruction *instr)
{
    const mips::Register *destReg = nullptr;
    bool hasResult = (instr->getResult()) ? true:false;

    const std::string &name = instr->getFunctionName();
    if (name == "print"){
        builtin_print(instr->getArguments());
    } else if (name == "read_char"){
        if (hasResult){
            destReg = activeFunction->Active()->getRegister(instr->getResult(),false);
            activeFunction->Active()->markChanged(destReg);
        } else {
            destReg = mips.getZero();
        }
        activeFunction->Active()->addInstruction("READ_CHAR", *destReg);
    } else if (name == "read_int"){
        if (hasResult){
            destReg = activeFunction->Active()->getRegister(instr->getResult(),false);
            activeFunction->Active()->markChanged(destReg);
        } else {
            destReg = mips.getZero();
        }
        activeFunction->Active()->addInstruction("READ_INT", *destReg);
    } else if (name == "read_string"){
        if (hasResult){
            destReg = activeFunction->Active()->getRegister(instr->getResult(),false);
            activeFunction->Active()->markChanged(destReg);
            activeFunction->Active()->addInstruction("MOVE", *destReg, mips.getGlobalPointer());
            activeFunction->Active()->addInstruction("READ_STRING", *destReg, *(mips.getParamRegisters()[0]));
            activeFunction->Active()->addInstruction("ADD", mips.getGlobalPointer(), mips.getGlobalPointer(), *(mips.getParamRegisters()[0]));
            activeFunction->Active()->addInstruction("ADDIU", mips.getGlobalPointer(), mips.getGlobalPointer(), 1);
        } else {
            activeFunction->Active()->addInstruction("READ_STRING", mips.getGlobalPointer(), *(mips.getRetRegister()));
        }
    } else if (name == "get_at"){
        ir::Value *op1 = instr->getArguments()[0];
        ir::Value *op2 = instr->getArguments()[1];

        if(!hasResult){
            activeFunction->Active()->markUsed(op1);
            activeFunction->Active()->markUsed(op2);
            return;
        }
        ir::Value *dest = instr->getResult();

        const mips::Register *op1Reg = activeFunction->Active()->getRegister(op1);
        const mips::Register *op2Reg = activeFunction->Active()->getRegister(op2);
        const mips::Register *destReg = activeFunction->Active()->getRegister(dest, false);
        activeFunction->Active()->markChanged(destReg);
        activeFunction->Active()->markUsed(op1);
        activeFunction->Active()->markUsed(op2);

        activeFunction->Active()->addInstruction("ADD", *destReg, *op1Reg, *op2Reg);
        activeFunction->Active()->addInstruction("LB", *destReg, 0, *destReg);

    } else if (name == "set_at"){

        ir::Value *op1 = instr->getArguments()[0];
        ir::Value *op2 = instr->getArguments()[1];
        ir::Value *op3 = instr->getArguments()[2];

        if(!hasResult){
            activeFunction->Active()->markUsed(op1);
            activeFunction->Active()->markUsed(op2);
            activeFunction->Active()->markUsed(op3);
            return;
        }
        ir::Value *dest = instr->getResult();

        const mips::Register *op1Reg = activeFunction->Active()->getRegister(op1);
        const mips::Register *op2Reg = activeFunction->Active()->getRegister(op2);
        const mips::Register *op3Reg = activeFunction->Active()->getRegister(op3);
        const mips::Register *destReg = activeFunction->Active()->getRegister(dest, false);
        activeFunction->Active()->markChanged(destReg);
        activeFunction->Active()->markUsed(op1);
        activeFunction->Active()->markUsed(op2);
        activeFunction->Active()->markUsed(op3);

        activeFunction->Active()->addInstruction("MOVE", *destReg, mips.getGlobalPointer()); // set ptr to new string
        activeFunction->Active()->addInstruction("MOVE", *(mips.getRetRegister()), *op1Reg);    // prepare source reg
        activeFunction->Active()->addInstruction("JAL", "$MOVE_R2_TO_GP$");            // copy
        activeFunction->Active()->addInstruction("ADD", *(mips.getRetRegister()), *destReg, *op2Reg);

        activeFunction->Active()->addInstruction("SB", *op3Reg, 0, *(mips.getRetRegister()));

    } else if (name == "strcat"){
        ir::Value *op1 = instr->getArguments()[0];
        ir::Value *op2 = instr->getArguments()[1];
        if(!hasResult){
            activeFunction->Active()->markUsed(op1);
            activeFunction->Active()->markUsed(op2);
            return;
        }
        ir::Value *dest = instr->getResult();

        const mips::Register *op1Reg = activeFunction->Active()->getRegister(op1);
        const mips::Register *op2Reg = activeFunction->Active()->getRegister(op2);
        const mips::Register *destReg = activeFunction->Active()->getRegister(dest, false);
        activeFunction->Active()->markChanged(destReg);
        activeFunction->Active()->markUsed(op1);
        activeFunction->Active()->markUsed(op2);

        activeFunction->Active()->addInstruction("MOVE", *destReg, mips.getGlobalPointer()); // set ptr to new string
        activeFunction->Active()->addInstruction("MOVE", *(mips.getRetRegister()), *op1Reg);    // prepare source reg
        activeFunction->Active()->addInstruction("JAL", "$MOVE_R2_TO_GP$");            // copy
        activeFunction->Active()->addInstruction("ADDI", mips.getGlobalPointer(), mips.getGlobalPointer(), -1); // concat
        activeFunction->Active()->addInstruction("MOVE", *(mips.getRetRegister()), *op2Reg);    // prepare source reg
        activeFunction->Active()->addInstruction("JAL", "$MOVE_R2_TO_GP$");            // copy
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
    activeFunction->Active()->markUsed(left);
    activeFunction->Active()->markUsed(right);

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
    activeFunction->Active()->markUsed(left);
    activeFunction->Active()->markUsed(right);

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
    activeFunction->Active()->markUsed(left);
    activeFunction->Active()->markUsed(right);

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
    activeFunction->Active()->markUsed(left);
    activeFunction->Active()->markUsed(right);

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
    activeFunction->Active()->markUsed(left);
    activeFunction->Active()->markUsed(right);

    activeFunction->Active()->addInstruction("DIV", *leftReg, *rightReg  );
    activeFunction->Active()->addInstruction("MFHI", *destReg );
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
    activeFunction->Active()->markUsed(left);
    activeFunction->Active()->markUsed(right);

    if (left->getDataType() == ir::Value::DataType::STRING){
        //$STR_CMP$
        activeFunction->Active()->addInstruction("MOVE", *(mips.getParamRegisters()[0]),*leftReg);
        activeFunction->Active()->addInstruction("MOVE", *(mips.getParamRegisters()[1]),*rightReg);
        activeFunction->Active()->addInstruction("JAL", "$STR_CMP$");
        activeFunction->Active()->addInstruction("SLT", *destReg, *mips.getRetRegister(),*mips.getZero());
    } else {
        activeFunction->Active()->addInstruction("SLT", *destReg,*leftReg, *rightReg);
    }
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
    activeFunction->Active()->markUsed(left);
    activeFunction->Active()->markUsed(right);

    const mips::Register *tempReg = activeFunction->Active()->getFreeRegister();

    if (left->getDataType() == ir::Value::DataType::STRING){
        //$STR_CMP$
        activeFunction->Active()->addInstruction("MOVE", *(mips.getParamRegisters()[0]),*leftReg);
        activeFunction->Active()->addInstruction("MOVE", *(mips.getParamRegisters()[1]),*rightReg);
        activeFunction->Active()->addInstruction("JAL", "$STR_CMP$");
        activeFunction->Active()->addInstruction("SLT", *destReg, *mips.getRetRegister(), *mips.getZero());
        activeFunction->Active()->addInstruction("SLTIU", *tempReg, *mips.getRetRegister(), 1);
        activeFunction->Active()->addInstruction("OR",    *destReg, *destReg, *tempReg);
    } else {
        activeFunction->Active()->addInstruction("SLT",   *destReg, *leftReg, *rightReg );
        activeFunction->Active()->addInstruction("XOR",   *tempReg, *leftReg, *rightReg );
        activeFunction->Active()->addInstruction("SLTIU", *tempReg, *tempReg, 1);
        activeFunction->Active()->addInstruction("OR",    *destReg, *destReg, *tempReg);
    }

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
    activeFunction->Active()->markUsed(left);
    activeFunction->Active()->markUsed(right);

    if (left->getDataType() == ir::Value::DataType::STRING){
        //$STR_CMP$
        activeFunction->Active()->addInstruction("MOVE", *(mips.getParamRegisters()[0]),*rightReg);
        activeFunction->Active()->addInstruction("MOVE", *(mips.getParamRegisters()[1]),*leftReg);
        activeFunction->Active()->addInstruction("JAL", "$STR_CMP$");
        activeFunction->Active()->addInstruction("SLT", *destReg, *mips.getRetRegister(),*mips.getZero());
    } else{
        activeFunction->Active()->addInstruction("SLT", *destReg,*rightReg, *leftReg);
    }
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
    activeFunction->Active()->markUsed(left);
    activeFunction->Active()->markUsed(right);

    const mips::Register *tempReg = activeFunction->Active()->getFreeRegister();

    if (left->getDataType() == ir::Value::DataType::STRING){
        //$STR_CMP$
        activeFunction->Active()->addInstruction("MOVE", *(mips.getParamRegisters()[1]),*leftReg);
        activeFunction->Active()->addInstruction("MOVE", *(mips.getParamRegisters()[0]),*rightReg);
        activeFunction->Active()->addInstruction("JAL", "$STR_CMP$");
        activeFunction->Active()->addInstruction("SLT", *destReg, *mips.getRetRegister(), *mips.getZero());
        activeFunction->Active()->addInstruction("SLTIU", *tempReg, *mips.getRetRegister(), 1);
        activeFunction->Active()->addInstruction("OR",    *destReg, *destReg, *tempReg);
    } else {
        activeFunction->Active()->addInstruction("SLT",   *destReg, *rightReg, *leftReg );
        activeFunction->Active()->addInstruction("XOR",   *tempReg, *rightReg, *leftReg );
        activeFunction->Active()->addInstruction("SLTIU", *tempReg, *tempReg, 1);
        activeFunction->Active()->addInstruction("OR",    *destReg, *destReg, *tempReg);
    }

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
    activeFunction->Active()->markUsed(left);
    activeFunction->Active()->markUsed(right);

    if (left->getDataType() == ir::Value::DataType::STRING){
        activeFunction->Active()->addInstruction("MOVE", *(mips.getParamRegisters()[1]),*leftReg);
        activeFunction->Active()->addInstruction("MOVE", *(mips.getParamRegisters()[0]),*rightReg);
        activeFunction->Active()->addInstruction("JAL", "$STR_CMP$");
         activeFunction->Active()->addInstruction("SLTIU", *destReg, *mips.getRetRegister(), 1);
    } else {
        activeFunction->Active()->addInstruction("XOR",*destReg, *leftReg, *rightReg);
        activeFunction->Active()->addInstruction("SLTIU", *destReg, *destReg, 1);
    }

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
    activeFunction->Active()->markUsed(left);
    activeFunction->Active()->markUsed(right);

    if (left->getDataType() == ir::Value::DataType::STRING){
        activeFunction->Active()->addInstruction("MOVE", *(mips.getParamRegisters()[1]),*leftReg);
        activeFunction->Active()->addInstruction("MOVE", *(mips.getParamRegisters()[0]),*rightReg);
        activeFunction->Active()->addInstruction("JAL", "$STR_CMP$");
        activeFunction->Active()->addInstruction("SLTU", *destReg, *mips.getZero(), *mips.getRetRegister());
    } else {
        activeFunction->Active()->addInstruction("XOR",*destReg, *leftReg, *rightReg);
        activeFunction->Active()->addInstruction("SLTU", *destReg, *mips.getZero(), *destReg);
    }
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
    activeFunction->Active()->markUsed(left);
    activeFunction->Active()->markUsed(right);

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
    activeFunction->Active()->markUsed(left);
    activeFunction->Active()->markUsed(right);

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
    activeFunction->Active()->markUsed(left);
    activeFunction->Active()->markUsed(right);


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
    activeFunction->Active()->markUsed(left);
    activeFunction->Active()->markUsed(right);


    activeFunction->Active()->addInstruction("OR", *destReg, *leftReg, *rightReg);
}

void ASMgenerator::visit(ir::NotInstruction *instr)
{
    ir::Value *op = instr->getOperand();
    ir::Value *dest = instr->getResult();

    const mips::Register *opReg = activeFunction->Active()->getRegister(op);
    const mips::Register *destReg = activeFunction->Active()->getRegister(dest, false);
    activeFunction->Active()->markChanged(destReg);
    activeFunction->Active()->markUsed(op);

    activeFunction->Active()->addInstruction("SLTIU", *destReg, *opReg, 1);

}

void ASMgenerator::visit(ir::TypecastInstruction *instr)
{
    ir::Value *op = instr->getOperand();
    ir::Value *dest = instr->getResult();
    const mips::Register *opReg = activeFunction->Active()->getRegister(op);
    const mips::Register *destReg = activeFunction->Active()->getRegister(dest, false);
    activeFunction->Active()->markChanged(destReg);
    activeFunction->Active()->markUsed(op);


    activeFunction->Active()->addCanonicalInstruction("#Typecast");

    switch(op->getDataType()){
    case ir::Value::DataType::CHAR:
    {
        switch(dest->getDataType()){
            case ir::Value::DataType::INT:
            {
                //char to int, simply move
                activeFunction->Active()->addInstruction("MOVE", *destReg, *opReg);
                break;
            }
            case ir::Value::DataType::STRING:
            {
                //char to str -- make new string str("c");
                activeFunction->Active()->addInstruction("MOVE", *destReg, mips.getGlobalPointer());
                activeFunction->Active()->addInstruction("SB", *opReg, 0,*destReg);
                activeFunction->Active()->addInstruction("SB", *mips.getZero(), 1, *destReg);
                activeFunction->Active()->addInstruction("ADDI", mips.getGlobalPointer(), mips.getGlobalPointer(), 2);
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
                activeFunction->Active()->addInstruction("ANDI", *destReg, *opReg, 0xFF);
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
    activeFunction->Active()->markUsed(op);

    activeFunction->Active()->addInstruction("NOR", *destReg, *opReg, *mips.getZero());
}

void ASMgenerator::visit(ir::NegInstruction *instr)
{
    ir::Value *op = instr->getOperand();
    ir::Value *dest = instr->getResult();

    const mips::Register *opReg = activeFunction->Active()->getRegister(op);
    const mips::Register *destReg = activeFunction->Active()->getRegister(dest, false);
    activeFunction->Active()->markChanged(destReg);
    activeFunction->Active()->markUsed(op);

    activeFunction->Active()->addInstruction("SUB", *destReg, *mips.getZero(),*opReg);
}

ConstStringData &ASMgenerator::getStringTable()
{
    return constStringData;
}

void ASMgenerator::builtin_print(std::vector<ir::Value *> &params)
{
    for(auto &item : params){
        const mips::Register *opReg = activeFunction->Active()->getRegister(item);
        activeFunction->Active()->markUsed(item);

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


