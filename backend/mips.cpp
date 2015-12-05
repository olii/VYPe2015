#include "mips.h"


namespace arch {

MIPS::MIPS():
    R0("$0",0),
    R1("$at",1),

    R2("$v0",2),
    R3("$v1",3),

    R4("$a0",4),
    R5("$a1",5),
    R6("$a2",6),
    R7("$a3",7),

    R8( "$t0",8),
    R9( "$t1",9),
    R10("$t2",10),
    R11("$t3",11),
    R12("$t4",12),
    R13("$t5",13),
    R14("$t6",14),
    R15("$t7",15),

    R16("$s0",16),
    R17("$s1",17),
    R18("$s2",18),
    R19("$s3",19),
    R20("$s4",20),
    R21("$s5",21),
    R22("$s6",22),
    R23("$s7",23),

    R24("$t8",24),
    R25("$t9",25),

    R28("$gp",28),
    R29("$sp",29),
    R30("$fp",30),
    R31{"$ra",31}
{


    calleeSavedRegisters = std::vector<Register *>{&R16, &R17, &R18, &R19, &R20, &R21, &R22, &R23};
    callerSavedRegisters = std::vector<Register *>{&R8, &R9, &R10, &R11, &R12, &R13, &R14, &R15, &R24, &R25};
    ParamRegisters       = std::vector<Register *>{&R4, &R5, &R6, &R7};

    EVALRegisters = callerSavedRegisters;
    EVALRegisters.insert(std::end(EVALRegisters), std::begin(calleeSavedRegisters), std::end(calleeSavedRegisters));

}

MIPS::~MIPS()
{

}

unsigned MIPS::getMaxRegisterParameters()
{
    return 4;
}

const arch::Register &MIPS::getStackPointer()
{
    return R29;
}

const arch::Register &MIPS::getFramePointer()
{
    return R30;
}

const arch::Register &MIPS::getRAPointer()
{
    return R31;
}

const std::vector<Register *> &MIPS::getParamRegisters()
{
    return ParamRegisters;
}


const std::vector<Register *> &MIPS::getEvalRegisters()
{
    return EVALRegisters;
}

const std::vector<Register *> &MIPS::getCalleeSavedRegisters()
{
    return calleeSavedRegisters;
}

const std::vector<Register *> &MIPS::getCallerSavedRegisters()
{
    return callerSavedRegisters;
}

const std::string MIPS::getFunctionPrologue()
{
    std::string prologue = "sub $sp, $sp, 8 \n";
    prologue +=            "sw $ra, 4($sp)  \n";
    prologue +=            "sw $fp, 0($sp)  \n";
    prologue +=            "move $fp, $sp  \n";

    return prologue;
}


const std::string MIPS::getFunctionEpilogue()
{
    std::string prologue = "lw $fp, 0($sp)  \n";
    prologue +=            "lw $ra, 4($sp)  \n";
    prologue +=            "add $sp, $sp, 8 \n";
    prologue +=            "jr $ra          \n";

    return prologue;
}


}
