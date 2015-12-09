#include "asmgenerator.h"
#include <iostream>


namespace backend {
ASMgenerator::ASMgenerator()
{

    std::cout << "<< ASMgenerator constructor spawned >>" << std::endl;
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
    //std::cout << "<< Entering function " << func->getName() << " >>" << std::endl;


    const std::vector<ir::Value*> parameters = func->getParameters();
    unsigned paramCount = parameters.size();

    //create new context
    FunctionContext fc(func);
    //store it by value
    context.insert(std::make_pair(func, fc));
    // set pointer to newly added reference
    activeFunction = &(context[func]);




    for (unsigned int i = 0; i<parameters.size(); i++)
    {
        // TODO: static cast
        ir::NamedValue* param = dynamic_cast<ir::NamedValue*>(parameters[i]);
        if (param == nullptr){
            // error handling ....
        }
        activeFunction->addVar(*param, i+1, paramCount);

    }

    // iterate over each basicblock
    for (ir::BasicBlock* bb : func->getBasicBlocks())
    {
        bb->accept(*this);
    }

    std::cout << context[func].getInstructions();
}

void ASMgenerator::visit(ir::BasicBlock *block)
{
    activeFunction->addBlock(block);

    for (ir::Instruction* inst : block->getInstructions())
            inst->accept(*this);

}

void ASMgenerator::visit(ir::NamedValue *value)
{
    activeFunction->Active()->getRegister(value);
}

void ASMgenerator::visit(ir::TemporaryValue *value)
{
    activeFunction->Active()->getRegister(value);
}

void ASMgenerator::visit(ir::ConstantValue<int> *value)
{

}

void ASMgenerator::visit(ir::ConstantValue<char> *value)
{

}

void ASMgenerator::visit(ir::ConstantValue<std::string> *value)
{

}

void ASMgenerator::visit(ir::AssignInstruction *instr)
{
    ir::Value *dest = instr->getResult();
    ir::Value *operand = instr->getOperand();

    arch::Register *operandReg = activeFunction->Active()->getRegister(operand, true); // must be locked
    arch::Register *destReg = activeFunction->Active()->getRegister(dest,false, false);
    activeFunction->Active()->unlockVar(operand);
    activeFunction->Active()->markChanged(destReg);

    activeFunction->Active()->addInstruction("move", destReg->getIDName(), 0, operandReg->getIDName() );


}

void ASMgenerator::visit(ir::DeclarationInstruction *instr)
{
    ir::Value *operand = instr->getOperand();
    activeFunction->addVar(*(static_cast<ir::NamedValue*>(operand)));
    // activeFunction->Active()->getRegister(operand);
}

void ASMgenerator::visit(ir::JumpInstruction *instr)
{
    activeFunction->Active()->saveUnsavedVariables();
    ir::BasicBlock *jumpBlock = instr->getFollowingBasicBlock();
    activeFunction->Active()->addInstruction("j", BlockContext::getName(jumpBlock,activeFunction->getFunction()));



}

void ASMgenerator::visit(ir::CondJumpInstruction *instr)
{
    activeFunction->Active()->saveUnsavedVariables();

    ir::Value *cond =  instr->getCondition();
    ir::BasicBlock *trueJump = instr->getTrueBasicBlock();
    ir::BasicBlock *falseJump = instr->getFalseBasicBlock();

    std::string trueJumpBlockName = activeFunction->getFunction()->getName() + "_$" + std::to_string(trueJump->getId());
    std::string falseJumpBlockName = activeFunction->getFunction()->getName() + "_$" + std::to_string(falseJump->getId());

    arch::Register *condReg = activeFunction->Active()->getRegister(cond);

    activeFunction->Active()->addInstruction("bne", condReg->getIDName(), 0, "$0",0,trueJumpBlockName);
    activeFunction->Active()->addInstruction("b ", falseJumpBlockName);
}

void ASMgenerator::visit(ir::ReturnInstruction *instr)
{
    ir::Value *retVal = instr->getOperand();
    arch::Register *retReg = activeFunction->Active()->getRegister(retVal);


    activeFunction->Active()->addInstruction("move", activeFunction->getMips()->getRetRegister()->getIDName(),
                                             0, retReg->getIDName() );

    activeFunction->Active()->addCanonicalInstruction("j " + activeFunction->getFunction()->getName() + "_$return\n");
}

void ASMgenerator::visit(ir::CallInstruction *instr)
{

    //save all registers with namedValue + temoraries which are locked
    std::vector<std::pair<ir::Value*,int>> activeMapping = activeFunction->Active()->saveCallerRegisters();
    int mappingSize = activeMapping.size();


    bool hasStackTransfer = instr->getArguments().size() > arch.getParamRegisters().size();
    if (hasStackTransfer)
    {
        // some parameters are stack transfered -> prepare place
        int requiredSize = (instr->getArguments().size() - arch.getParamRegisters().size()) *4;
        activeFunction->Active()->addInstruction("addi", "$sp", 0, "$sp", 0, std::to_string(-requiredSize));
    }

    int stackOffset = 0;
    unsigned int i = 1;
    for(auto &param : instr->getArguments()){
        arch::Register *paramReg = activeFunction->Active()->getRegister(param);

        if (i<=arch.getMaxRegisterParameters()) {
            arch::Register *destReg = arch.getParamRegisters()[i-1];

            activeFunction->Active()->addInstruction("move", destReg->getIDName(), 0, paramReg->getIDName());
        }  else {
            // cdecl convention
            activeFunction->Active()->addInstruction("sw", paramReg->getIDName(), 0, std::to_string(stackOffset) + "($sp)");
            stackOffset += 4;
        }
        i++;

        // if parameter was temporary, remove it from activeMapping
        //search for value in activeMapping
        for (auto &item: activeMapping) {
            if ( param == item.first )
            {
                item.first = nullptr;
            }
        }

    }

    activeFunction->Active()->clearCallerRegisters();

    // function call
    activeFunction->Active()->addInstruction("jal", instr->getFunction()->getName());

    // return stack to valid state
    if (hasStackTransfer)
    {
        int requiredSize = (instr->getArguments().size() - arch.getParamRegisters().size()) *4;
        activeFunction->Active()->addInstruction("addi", "$sp", 0, "$sp", 0, std::to_string(requiredSize));
    }

    // recover old mapping
    activeFunction->Active()->loadMapingAfterCall(activeMapping, mappingSize);

    if (instr->getResult())
    {
        // save result if has one
        arch::Register *destReg = activeFunction->Active()->getRegister(instr->getResult(),false, false);
        activeFunction->Active()->markChanged(destReg);

        activeFunction->Active()->addInstruction("move", destReg->getIDName(), 0, arch.getRetRegister()->getIDName());
    }


}

void ASMgenerator::visit(ir::BuiltinCallInstruction *instr)
{
	// TODO: IMPLEMENT ME
}

void ASMgenerator::visit(ir::AddInstruction *instr)
{

    ir::Value *left = instr->getLeftOperand();
    ir::Value *right = instr->getRightOperand();
    ir::Value *dest = instr->getResult();

    arch::Register *leftReg = nullptr;
    arch::Register *rightReg = nullptr;


    /*if ( left->getType() == ir::Value::Type::TEMPORARY )
    {*/
        leftReg = activeFunction->Active()->getRegister(left, true);
        rightReg = activeFunction->Active()->getRegister(right);
        activeFunction->Active()->unlockVar(left); // unlocking left
    /*}
    else
    {
        rightReg = activeFunction->Active()->getRegister(right, true);
        leftReg = activeFunction->Active()->getRegister(left);
        activeFunction->Active()->unlockVar(right); // unlocking left
    }*/


    arch::Register *destReg = activeFunction->Active()->getRegister(dest);
    activeFunction->Active()->markChanged(destReg);

    activeFunction->Active()->addInstruction("ADD", destReg->getIDName(),
                                             0, leftReg->getIDName(), 0, rightReg->getIDName() );

}

void ASMgenerator::visit(ir::SubtractInstruction *instr)
{
    ir::Value *left = instr->getLeftOperand();
    ir::Value *right = instr->getRightOperand();
    ir::Value *dest = instr->getResult();

    arch::Register *leftReg = nullptr;
    arch::Register *rightReg = nullptr;

    leftReg = activeFunction->Active()->getRegister(left, true);
    rightReg = activeFunction->Active()->getRegister(right);
    activeFunction->Active()->unlockVar(left); // unlocking left

    arch::Register *destReg = activeFunction->Active()->getRegister(dest);
    activeFunction->Active()->markChanged(destReg);

    activeFunction->Active()->addInstruction("SUB",destReg->getIDName(),
                                             0, leftReg->getIDName(), 0, rightReg->getIDName() );
}

void ASMgenerator::visit(ir::MultiplyInstruction *instr)
{
    ir::Value *left = instr->getLeftOperand();
    ir::Value *right = instr->getRightOperand();
    ir::Value *dest = instr->getResult();

    arch::Register *leftReg = nullptr;
    arch::Register *rightReg = nullptr;

    leftReg = activeFunction->Active()->getRegister(left, true);
    rightReg = activeFunction->Active()->getRegister(right);
    activeFunction->Active()->unlockVar(left); // unlocking left


    arch::Register *destReg = activeFunction->Active()->getRegister(dest);
    activeFunction->Active()->markChanged(destReg);

    activeFunction->Active()->addInstruction("MUL", destReg->getIDName(),
                                             0, leftReg->getIDName(), 0, rightReg->getIDName() );
}

void ASMgenerator::visit(ir::DivideInstruction *instr)
{

}

void ASMgenerator::visit(ir::ModuloInstruction *instr)
{

}

void ASMgenerator::visit(ir::LessInstruction *instr)
{
    ir::Value *left = instr->getLeftOperand();
    ir::Value *right = instr->getRightOperand();
    ir::Value *dest = instr->getResult();

    arch::Register *leftReg = nullptr;
    arch::Register *rightReg = nullptr;

    leftReg = activeFunction->Active()->getRegister(left, true);
    rightReg = activeFunction->Active()->getRegister(right);
    activeFunction->Active()->unlockVar(left); // unlocking left

    arch::Register *destReg = activeFunction->Active()->getRegister(dest);
    activeFunction->Active()->markChanged(destReg);

    activeFunction->Active()->addInstruction("SLT",destReg->getIDName(),
                                             0, leftReg->getIDName(), 0, rightReg->getIDName() );
}

void ASMgenerator::visit(ir::LessEqualInstruction *instr)
{
    ir::Value *left = instr->getLeftOperand();
    ir::Value *right = instr->getRightOperand();
    ir::Value *dest = instr->getResult();

    arch::Register *leftReg = nullptr;
    arch::Register *rightReg = nullptr;

    leftReg = activeFunction->Active()->getRegister(left, true);
    rightReg = activeFunction->Active()->getRegister(right);
    activeFunction->Active()->unlockVar(left); // unlocking left

    arch::Register *tempReg = activeFunction->Active()->getFreeRegister();
    arch::Register *destReg = activeFunction->Active()->getRegister(dest);
    activeFunction->Active()->markChanged(destReg);


    activeFunction->Active()->addInstruction("SLT",destReg->getIDName(),
                                             0, leftReg->getIDName(), 0, rightReg->getIDName() );

    activeFunction->Active()->addInstruction("XOR",tempReg->getIDName(),
                                             0, leftReg->getIDName(), 0, rightReg->getIDName() );
    activeFunction->Active()->addInstruction("SLTIU", tempReg->getIDName(),
                                             0, destReg->getIDName(), 0, "1");

    activeFunction->Active()->addInstruction("OR", destReg->getIDName(),
                                             0, destReg->getIDName(), 0, tempReg->getIDName());

}

void ASMgenerator::visit(ir::GreaterInstruction *instr)
{
    ir::Value *left = instr->getLeftOperand();
    ir::Value *right = instr->getRightOperand();
    ir::Value *dest = instr->getResult();

    arch::Register *leftReg = nullptr;
    arch::Register *rightReg = nullptr;

    leftReg = activeFunction->Active()->getRegister(left, true);
    rightReg = activeFunction->Active()->getRegister(right);
    activeFunction->Active()->unlockVar(left); // unlocking left

    arch::Register *destReg = activeFunction->Active()->getRegister(dest);
    activeFunction->Active()->markChanged(destReg);

    activeFunction->Active()->addInstruction("SLT",destReg->getIDName(),
                                             0, rightReg->getIDName(), 0, leftReg->getIDName() );
}

void ASMgenerator::visit(ir::GreaterEqualInstruction *instr)
{
    ir::Value *left = instr->getLeftOperand();
    ir::Value *right = instr->getRightOperand();
    ir::Value *dest = instr->getResult();

    arch::Register *leftReg = nullptr;
    arch::Register *rightReg = nullptr;

    leftReg = activeFunction->Active()->getRegister(left, true);
    rightReg = activeFunction->Active()->getRegister(right);
    activeFunction->Active()->unlockVar(left); // unlocking left

    arch::Register *tempReg = activeFunction->Active()->getFreeRegister();
    arch::Register *destReg = activeFunction->Active()->getRegister(dest);
    activeFunction->Active()->markChanged(destReg);


    activeFunction->Active()->addInstruction("SLT",destReg->getIDName(),
                                             0, rightReg->getIDName(), 0, leftReg->getIDName() );

    activeFunction->Active()->addInstruction("XOR",tempReg->getIDName(),
                                             0, rightReg->getIDName(), 0, leftReg->getIDName() );
    activeFunction->Active()->addInstruction("SLTIU", tempReg->getIDName(),
                                             0, destReg->getIDName(), 0, "1");

    activeFunction->Active()->addInstruction("OR", destReg->getIDName(),
                                             0, destReg->getIDName(), 0, tempReg->getIDName());
}

void ASMgenerator::visit(ir::EqualInstruction *instr)
{

    ir::Value *left = instr->getLeftOperand();
    ir::Value *right = instr->getRightOperand();
    ir::Value *dest = instr->getResult();

    arch::Register *leftReg = nullptr;
    arch::Register *rightReg = nullptr;


    leftReg = activeFunction->Active()->getRegister(left, true);
    rightReg = activeFunction->Active()->getRegister(right);
    activeFunction->Active()->unlockVar(left); // unlocking left


    arch::Register *destReg = activeFunction->Active()->getRegister(dest);


    activeFunction->Active()->markChanged(destReg);

    activeFunction->Active()->addInstruction("XOR",destReg->getIDName(),
                                             0, leftReg->getIDName(), 0, rightReg->getIDName() );

    activeFunction->Active()->addInstruction("SLTIU", destReg->getIDName(),
                                             0, destReg->getIDName(), 0, "1");

}

void ASMgenerator::visit(ir::NotEqualInstruction *instr)
{
    ir::Value *left = instr->getLeftOperand();
    ir::Value *right = instr->getRightOperand();
    ir::Value *dest = instr->getResult();

    arch::Register *leftReg = nullptr;
    arch::Register *rightReg = nullptr;


    leftReg = activeFunction->Active()->getRegister(left, true);
    rightReg = activeFunction->Active()->getRegister(right);
    activeFunction->Active()->unlockVar(left); // unlocking left


    arch::Register *destReg = activeFunction->Active()->getRegister(dest);
    activeFunction->Active()->markChanged(destReg);

    activeFunction->Active()->addInstruction("XOR",destReg->getIDName(),
                                             0, leftReg->getIDName(), 0, rightReg->getIDName() );

    activeFunction->Active()->addInstruction("SLTIU", destReg->getIDName(),
                                             0, destReg->getIDName(), 0, "1");

    activeFunction->Active()->addInstruction("SLTIU", destReg->getIDName(),
                                             0, destReg->getIDName(), 0, "1");
}

void ASMgenerator::visit(ir::AndInstruction *instr)
{
    ir::Value *left = instr->getLeftOperand();
    ir::Value *right = instr->getRightOperand();
    ir::Value *dest = instr->getResult();

    arch::Register *leftReg = nullptr;
    arch::Register *rightReg = nullptr;

    leftReg = activeFunction->Active()->getRegister(left, true);
    rightReg = activeFunction->Active()->getRegister(right);
    activeFunction->Active()->unlockVar(left); // unlocking left

    arch::Register *destReg = activeFunction->Active()->getRegister(dest);
    activeFunction->Active()->markChanged(destReg);

    activeFunction->Active()->addInstruction("AND",destReg->getIDName(),
                                             0, leftReg->getIDName(), 0, rightReg->getIDName() );
}

void ASMgenerator::visit(ir::OrInstruction *instr)
{
    ir::Value *left = instr->getLeftOperand();
    ir::Value *right = instr->getRightOperand();
    ir::Value *dest = instr->getResult();

    arch::Register *leftReg = nullptr;
    arch::Register *rightReg = nullptr;

    leftReg = activeFunction->Active()->getRegister(left, true);
    rightReg = activeFunction->Active()->getRegister(right);
    activeFunction->Active()->unlockVar(left); // unlocking left
    activeFunction->Active()->unlockVar(right);

    arch::Register *destReg = activeFunction->Active()->getRegister(dest);
    activeFunction->Active()->markChanged(destReg);

    activeFunction->Active()->addInstruction("OR",destReg->getIDName(),
                                             0, leftReg->getIDName(), 0, rightReg->getIDName() );
}

void ASMgenerator::visit(ir::NotInstruction *instr)
{
    ir::Value *op = instr->getOperand();
    ir::Value *result = instr->getResult();

    arch::Register *opReg = activeFunction->Active()->getRegister(op);
    arch::Register *destReg = activeFunction->Active()->getRegister(result);
    activeFunction->Active()->markChanged(destReg);

    activeFunction->Active()->addInstruction("SLTIU", destReg->getIDName(),
                                             0, opReg->getIDName(), 0, "1");


}

void ASMgenerator::visit(ir::TypecastInstruction *instr)
{

}

}


