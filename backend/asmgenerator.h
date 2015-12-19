#ifndef ASMGENERATOR_H
#define ASMGENERATOR_H
#include "ir/ir_visitor.h"
#include "ir/builder.h"
#include "ir/function.h"
#include "mips.h"
#include <list>
#include "functioncontext.h"
#include "backend/conststringdata.h"

namespace backend {


class ASMgenerator : public ir::IrVisitor
{
public:
    ASMgenerator();
    virtual ~ASMgenerator();
    int translateIR(ir::Builder &builder);
    std::stringstream getTargetCode();

    /* IR visitor pure virtual methods */
    virtual void visit(ir::Function* func) override;
    virtual void visit(ir::BasicBlock* block) override;
    virtual void visit(ir::NamedValue* value) override;
    virtual void visit(ir::TemporaryValue* value) override;
    virtual void visit(ir::ConstantValue<int>* value) override;
    virtual void visit(ir::ConstantValue<char>* value) override;
    virtual void visit(ir::ConstantValue<std::string>* value) override;
    virtual void visit(ir::AssignInstruction* instr) override;
    virtual void visit(ir::DeclarationInstruction* instr) override;
    virtual void visit(ir::JumpInstruction* instr) override;
    virtual void visit(ir::CondJumpInstruction* instr) override;
    virtual void visit(ir::ReturnInstruction* instr) override;
    virtual void visit(ir::CallInstruction* instr) override;
    virtual void visit(ir::BuiltinCallInstruction* instr) override;
    virtual void visit(ir::AddInstruction* instr) override;
    virtual void visit(ir::SubtractInstruction* instr) override;
    virtual void visit(ir::MultiplyInstruction* instr) override;
    virtual void visit(ir::DivideInstruction* instr) override;
    virtual void visit(ir::ModuloInstruction* instr) override;
    virtual void visit(ir::LessInstruction* instr) override;
    virtual void visit(ir::LessEqualInstruction* instr) override;
    virtual void visit(ir::GreaterInstruction* instr) override;
    virtual void visit(ir::GreaterEqualInstruction* instr) override;
    virtual void visit(ir::EqualInstruction* instr) override;
    virtual void visit(ir::NotEqualInstruction* instr) override;
    virtual void visit(ir::AndInstruction* instr) override;
    virtual void visit(ir::OrInstruction* instr) override;
    virtual void visit(ir::BitwiseAndInstruction* instr) override;
    virtual void visit(ir::BitwiseOrInstruction* instr) override;
    virtual void visit(ir::NotInstruction* instr) override;
    virtual void visit(ir::TypecastInstruction* instr) override;
    virtual void visit(ir::BitwiseNotInstruction* instr) override;
    virtual void visit(ir::NegInstruction* instr) override;


    /**/
    ConstStringData &getStringTable();

private:
    mips::MIPS mips;
    std::map<ir::Function*,FunctionContext> context;
    FunctionContext* activeFunction;


    ConstStringData constStringData;

    void builtin_print(std::vector<ir::Value*> &params);
};


}

#endif // ASMGENERATOR_H
