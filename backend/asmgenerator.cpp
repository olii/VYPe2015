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
    std::cout << "<< Entering function " << func->getName() << " >>" << std::endl;


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
        ir::NamedValue* param = dynamic_cast<ir::NamedValue*>(parameters[i]);
        if (param == nullptr){
            // error handling ....
        }

        std::cout << "Parameter [" << param->getName() << "]" << std::endl;

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
//	for (BasicBlock* pred : block->getPredecessors())
//	{
//		pred->getId();
//	}


//	for (BasicBlock* succ : block->getSuccessors())
//	{

//		succ->getId();
//	}
//	_os << (any ? "" : prefix) << "] ";


//	for (Value* def : block->getDefs())
//	{
//		def->accept(*this);
//	}


//	for (Value* use : block->getUses())
//	{
//		use->accept(*this);
//	}

//	for (Instruction* inst : block->getInstructions())
//		inst->accept(*this);

    //activeContext->addInstruction(std::to_string(block->getId()) + ":");  // add label to function

    activeFunction->addBlock(block);

    for (ir::Instruction* inst : block->getInstructions())
            inst->accept(*this);
}

void ASMgenerator::visit(ir::NamedValue *value)
{

}

void ASMgenerator::visit(ir::TemporaryValue *value)
{

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

}

void ASMgenerator::visit(ir::DeclarationInstruction *instr)
{
}

void ASMgenerator::visit(ir::JumpInstruction *instr)
{

}

void ASMgenerator::visit(ir::CondJumpInstruction *instr)
{

}

void ASMgenerator::visit(ir::ReturnInstruction *instr)
{

}

void ASMgenerator::visit(ir::CallInstruction *instr)
{

}

void ASMgenerator::visit(ir::AddInstruction *instr)
{

}

void ASMgenerator::visit(ir::SubtractInstruction *instr)
{

}

void ASMgenerator::visit(ir::MultiplyInstruction *instr)
{

}

void ASMgenerator::visit(ir::DivideInstruction *instr)
{

}

void ASMgenerator::visit(ir::ModuloInstruction *instr)
{

}

void ASMgenerator::visit(ir::LessInstruction *instr)
{

}

void ASMgenerator::visit(ir::LessEqualInstruction *instr)
{

}

void ASMgenerator::visit(ir::GreaterInstruction *instr)
{

}

void ASMgenerator::visit(ir::GreaterEqualInstruction *instr)
{

}

void ASMgenerator::visit(ir::EqualInstruction *instr)
{

}

void ASMgenerator::visit(ir::NotEqualInstruction *instr)
{

}

void ASMgenerator::visit(ir::AndInstruction *instr)
{

}

void ASMgenerator::visit(ir::OrInstruction *instr)
{

}

void ASMgenerator::visit(ir::NotInstruction *instr)
{

}

void ASMgenerator::visit(ir::TypecastInstruction *instr)
{

}

}


