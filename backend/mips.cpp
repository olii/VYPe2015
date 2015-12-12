#include "mips.h"


namespace mips {

const std::string Indent = "        ";




MIPS::MIPS():
    R0("0",0),
    R1("at",1),

    R2("v0",2),
    R3("v1",3),

    R4("a0",4),
    R5("a1",5),
    R6("a2",6),
    R7("a3",7),

    R8( "t0",8),
    R9( "t1",9),
    R10("t2",10),
    R11("t3",11),
    R12("t4",12),
    R13("t5",13),
    R14("t6",14),
    R15("t7",15),

    R16("s0",16),
    R17("s1",17),
    R18("s2",18),
    R19("s3",19),
    R20("s4",20),
    R21("s5",21),
    R22("s6",22),
    R23("s7",23),

    R24("t8",24),
    R25("t9",25),

    R28("gp",28, true),
    R29("sp",29, true),
    R30("fp",30, true),
    R31{"ra",31, true}
{


    calleeSavedRegisters = std::vector<const Register *>{&R16, &R17, &R18, &R19, &R20, &R21, &R22, &R23};
    callerSavedRegisters = std::vector<const Register *>{&R8, &R9, &R10, &R11, &R12, &R13, &R14, &R15, &R24, &R25};
    ParamRegisters       = std::vector<const Register *>{&R4, &R5, &R6, &R7};

    EVALRegisters = callerSavedRegisters;
    EVALRegisters.insert(std::end(EVALRegisters), std::begin(calleeSavedRegisters), std::end(calleeSavedRegisters));

}

MIPS::~MIPS()
{

}

const Register &MIPS::getStackPointer() const
{
    return R29;
}

const Register &MIPS::getFramePointer() const
{
    return R30;
}

const Register &MIPS::getRAPointer() const
{
    return R31;
}

const std::vector<const Register *> &MIPS::getParamRegisters() const
{
    return ParamRegisters;
}


const std::vector<const Register *> &MIPS::getEvalRegisters() const
{
    return EVALRegisters;
}

const std::vector<const Register *> &MIPS::getCalleeSavedRegisters() const
{
    return calleeSavedRegisters;
}

const std::vector<const Register *> &MIPS::getCallerSavedRegisters() const
{
    return callerSavedRegisters;
}

const Register *MIPS::getRetRegister() const
{
    return &R2;
}

const Register *MIPS::getZero() const
{
    return &R0;
}

const std::string MIPS::getFunctionPrologue() const
{
    std::string prologue = "  addi $sp, $sp, -8 \n";
    prologue +=            "  sw $ra, 4($sp)  \n";
    prologue +=            "  sw $fp, 0($sp)  \n";
    prologue +=            "  move $fp, $sp  \n";

    return prologue;
}


const std::string MIPS::getFunctionEpilogue() const
{
    std::string prologue = "  lw $ra, 4($fp)  \n";
    prologue +=            "  move $sp, $fp  \n";
    prologue +=            "  lw $fp, 0($fp)  \n";
    prologue +=            "  addi $sp, $sp, 8 \n";
    prologue +=            "  jr $ra          \n";

    return prologue;
}


}
