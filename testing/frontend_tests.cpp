#include "gtest/gtest.h"
#include "frontend/symbol.h"

using namespace ::testing;
using namespace frontend;

class SymbolTest : public Test {};

TEST_F(SymbolTest,
StringToDataType) {
    EXPECT_EQ(Symbol::DataType::INT, Symbol::stringToDataType("int"));
    EXPECT_EQ(Symbol::DataType::CHAR, Symbol::stringToDataType("char"));
    EXPECT_EQ(Symbol::DataType::STRING, Symbol::stringToDataType("string"));
    EXPECT_EQ(Symbol::DataType::VOID, Symbol::stringToDataType("void"));
    EXPECT_EQ(Symbol::DataType::VOID, Symbol::stringToDataType("no specific type"));
}

TEST_F(SymbolTest,
GetType) {
    VariableSymbol varSymbol("varSymbol", Symbol::DataType::STRING);
    FunctionSymbol funcSymbol("funcSymbol", Symbol::DataType::STRING, {});

    EXPECT_EQ(Symbol::Type::VARIABLE, varSymbol.getType());
    EXPECT_EQ(Symbol::Type::FUNCTION, funcSymbol.getType());
}

TEST_F(SymbolTest,
GetName) {
    VariableSymbol varSymbol("varSymbol", Symbol::DataType::STRING);
    FunctionSymbol funcSymbol("funcSymbol", Symbol::DataType::STRING, {});

    EXPECT_EQ("varSymbol", varSymbol.getName());
    EXPECT_EQ("funcSymbol", funcSymbol.getName());
}

