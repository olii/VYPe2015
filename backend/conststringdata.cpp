#include "conststringdata.h"
#include <string>

ConstStringData::ConstStringData()
{
}

const std::string &ConstStringData::getLabel(ir::Value *val)
{
    if (table.find(val) != table.end()){
        // found
        return table.at(val);
    }
    // find by value
    for( auto &it : table){
        ir::ConstantValue<std::string> *tmp = static_cast<ir::ConstantValue<std::string>*>(it.first);
        ir::ConstantValue<std::string> *cVal = static_cast<ir::ConstantValue<std::string>*>(val);
        if (tmp->getConstantValue() == cVal->getConstantValue()){
            return it.second;
        }
    }


    std::string label = "String_" + std::to_string(table.size()) ;

    table.emplace(val, label);
    return table.at(val);
}

std::stringstream ConstStringData::TranslateTable()
{
    std::stringstream out;

    for( auto &it : table){
        ir::ConstantValue<std::string> *tmp = static_cast<ir::ConstantValue<std::string>*>(it.first);
        out << it.second << ": .string \"" << tmp->getConstantValue() << "\""  << "\n";
    }
    out << "DATABEGIN:\n";
    return out;
}

