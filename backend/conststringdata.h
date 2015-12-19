#ifndef CONSTSTRINGDATA_H
#define CONSTSTRINGDATA_H
#include <map>
#include "ir/value.h"
#include <sstream>

class ConstStringData
{
public:
    ConstStringData();

    const std::string &getLabel(ir::Value *val);
    std::stringstream TranslateTable();

private:
    std::map<ir::Value*, std::string> table;
};

#endif // CONSTSTRINGDATA_H
