#ifndef MIPS_H
#define MIPS_H

#include <string>
#include <vector>





namespace arch{



class Register{
public:
    Register(std::string name, int id):_name(name),_id(id){}
    const std::string & getName(){return _name;}
    unsigned getID(){return _id;}
private:
        const std::string _name;
        const unsigned _id;
};



class MIPS
{
public:
    MIPS();
     ~MIPS();
    unsigned getMaxRegisterParameters();
    const arch::Register& getStackPointer();
    const arch::Register& getFramePointer();
    const arch::Register& getRAPointer();
    const std::vector<Register *>& getParamRegisters();

    const std::string getFunctionPrologue();
    const std::string getFunctionEpilogue();

    const std::vector<Register *> &getEvalRegisters();
    const std::vector<Register *> &getCalleeSavedRegisters();
    const std::vector<Register *> &getCallerSavedRegisters();

private:
    arch::Register R0; // always 0
    arch::Register R1; // reserved for assembler

    arch::Register R2; // return value + eval
    arch::Register R3; // R2

    arch::Register R4; // PARAM
    arch::Register R5;
    arch::Register R6;
    arch::Register R7;

    arch::Register R8; // EVAL caller saved
    arch::Register R9;
    arch::Register R10;
    arch::Register R11;
    arch::Register R12;
    arch::Register R13;
    arch::Register R14;
    arch::Register R15;


    arch::Register R16; // EVAL calle saved registers
    arch::Register R17;
    arch::Register R18;
    arch::Register R19;
    arch::Register R20;
    arch::Register R21;
    arch::Register R22;
    arch::Register R23;

    arch::Register R24; // EVAL
    arch::Register R25;

    arch::Register R28; // GP
    arch::Register R29; // SP
    arch::Register R30; // FramePointer, alt. EBP
    arch::Register R31; // Return Address register

    std::vector<Register *> calleeSavedRegisters;
    std::vector<Register *> callerSavedRegisters;
    std::vector<Register *> EVALRegisters;
    std::vector<Register *> ParamRegisters;
};

}

#endif // MIPS_H
