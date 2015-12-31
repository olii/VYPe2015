/**
 * Project VYPe 2015/2016
 * Marek Milkovič (xmilko01) & Oliver Nemček (xnemce03)
 **/

#ifndef MIPS_H
#define MIPS_H

#include <string>
#include <vector>





namespace mips{
extern const std::string Indent;




class Register{
public:
    Register(std::string name, int id, bool alias = false):_name(name),_id(id), use_alias_name(alias){}
    const std::string getName() const {return "$" + _name;}
    unsigned getID() const        {return _id;}
    const std::string getAsmName() const {return "$" + ((use_alias_name)?_name:std::to_string(_id)) ;}
private:
        const std::string _name;
        const unsigned _id;
        const bool use_alias_name;
};



class MIPS
{
public:
    MIPS();
     ~MIPS();
    const Register& getStackPointer() const;
    const Register& getFramePointer() const;
    const Register &getGlobalPointer() const;
    const Register& getRAPointer() const;
    const std::vector<const Register *>& getParamRegisters() const;

    const std::string getFunctionPrologue() const;
    const std::string getFunctionEpilogue() const;

    const std::vector<const Register *> &getEvalRegisters() const;
    const std::vector<const Register *> &getCalleeSavedRegisters() const;
    const std::vector<const Register *> &getCallerSavedRegisters() const;
    const Register *getRetRegister() const;
    const Register *getZero() const;

private:
    const Register R0; // always 0
    const Register R1; // reserved for assembler

    const Register R2; // return value + eval
    const Register R3; // R2

    const Register R4; // PARAM
    const Register R5;
    const Register R6;
    const Register R7;

    const Register R8; // EVAL caller saved
    const Register R9;
    const Register R10;
    const Register R11;
    const Register R12;
    const Register R13;
    const Register R14;
    const Register R15;


    const Register R16; // EVAL calle saved registers
    const Register R17;
    const Register R18;
    const Register R19;
    const Register R20;
    const Register R21;
    const Register R22;
    const Register R23;

    const Register R24; // EVAL
    const Register R25;

    const Register R28; // GP
    const Register R29; // SP
    const Register R30; // FramePointer, alt. EBP
    const Register R31; // Return Address register

    std::vector<const Register *> calleeSavedRegisters;
    std::vector<const Register *> callerSavedRegisters;
    std::vector<const Register *> EVALRegisters;
    std::vector<const Register *> ParamRegisters;
};

}

#endif // MIPS_H
