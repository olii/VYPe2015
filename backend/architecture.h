/**
 * Project VYPe 2015/2016
 * Marek Milkovič (xmilko01) & Oliver Nemček (xnemce03)
 **/

#ifndef ARCHITECTURE_H
#define ARCHITECTURE_H


#include <string>
#include <vector>

namespace arch {


class Register{
public:
    Register(std::string name, int id):_name(name),_id(id){}
    const std::string & getName(){return _name;}
    unsigned getID(){return _id;}
private:
        const std::string _name;
        const unsigned _id;
};




class Architecture
{
public:
    virtual ~Architecture() {}

    virtual unsigned getMaxRegisterParameters() = 0;
    virtual const Register& getStackPointer() = 0;
    virtual const Register& getFramePointer() = 0;
    virtual const Register& getRAPointer() = 0;
    virtual const std::vector<Register*> getParamRegisters() = 0;

protected:
    Architecture() {}
    Architecture( const Architecture& ) {}
};

}

#endif // ARCHITECTURE_H
