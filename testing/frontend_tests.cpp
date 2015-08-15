#include "gtest/gtest.h"
#include "frontend/context.h"
#include "frontend/symbol.h"
#include "frontend/symbol_table.h"

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
    FunctionSymbol funcSymbol("funcSymbol", Symbol::DataType::STRING, {}, false);

    EXPECT_EQ(Symbol::Type::VARIABLE, varSymbol.getType());
    EXPECT_EQ(Symbol::Type::FUNCTION, funcSymbol.getType());
}

TEST_F(SymbolTest,
GetName) {
    VariableSymbol varSymbol("varSymbol", Symbol::DataType::STRING);
    FunctionSymbol funcSymbol("funcSymbol", Symbol::DataType::STRING, {}, false);

    EXPECT_EQ("varSymbol", varSymbol.getName());
    EXPECT_EQ("funcSymbol", funcSymbol.getName());
}

class VariableSymbolTest : public Test {};

TEST_F(VariableSymbolTest,
GetDataType) {
    VariableSymbol intSymbol("intSymbol", Symbol::DataType::INT);
    VariableSymbol charSymbol("charSymbol", Symbol::DataType::CHAR);
    VariableSymbol stringSymbol("stringSymbol", Symbol::DataType::STRING);

    EXPECT_EQ(Symbol::DataType::INT, intSymbol.getDataType());
    EXPECT_EQ(Symbol::DataType::CHAR, charSymbol.getDataType());
    EXPECT_EQ(Symbol::DataType::STRING, stringSymbol.getDataType());
}

TEST_F(VariableSymbolTest,
SetDataType) {
    VariableSymbol intSymbol("intSymbol", Symbol::DataType::INT);
    EXPECT_EQ(Symbol::DataType::INT, intSymbol.getDataType());

    intSymbol.setDataType(Symbol::DataType::CHAR);
    EXPECT_EQ(Symbol::DataType::CHAR, intSymbol.getDataType());

    intSymbol.setDataType(Symbol::DataType::STRING);
    EXPECT_EQ(Symbol::DataType::STRING, intSymbol.getDataType());
}

class FunctionSymbolTest : public Test {};

TEST_F(FunctionSymbolTest,
GetReturnType) {
    FunctionSymbol funcVoid("function", Symbol::DataType::VOID, {}, false);
    FunctionSymbol funcInt("function", Symbol::DataType::INT, {}, false);
    FunctionSymbol funcChar("function", Symbol::DataType::CHAR, {}, false);
    FunctionSymbol funcString("function", Symbol::DataType::STRING, {}, false);

    EXPECT_EQ(Symbol::DataType::VOID, funcVoid.getReturnType());
    EXPECT_EQ(Symbol::DataType::INT, funcInt.getReturnType());
    EXPECT_EQ(Symbol::DataType::CHAR, funcChar.getReturnType());
    EXPECT_EQ(Symbol::DataType::STRING, funcString.getReturnType());
}

TEST_F(FunctionSymbolTest,
SetReturnType) {
    FunctionSymbol funcVoid("function", Symbol::DataType::VOID, {}, false);
    EXPECT_EQ(Symbol::DataType::VOID, funcVoid.getReturnType());

    funcVoid.setReturnType(Symbol::DataType::INT);
    EXPECT_EQ(Symbol::DataType::INT, funcVoid.getReturnType());

    funcVoid.setReturnType(Symbol::DataType::CHAR);
    EXPECT_EQ(Symbol::DataType::CHAR, funcVoid.getReturnType());

    funcVoid.setReturnType(Symbol::DataType::STRING);
    EXPECT_EQ(Symbol::DataType::STRING, funcVoid.getReturnType());
}

TEST_F(FunctionSymbolTest,
GetParameters) {
    FunctionSymbol func1("function", Symbol::DataType::VOID, {}, false);
    FunctionSymbol func2("function", Symbol::DataType::VOID, { FunctionSymbol::Parameter("a", Symbol::DataType::INT) }, false);

    EXPECT_EQ(0u, func1.getParameters().size());
    EXPECT_EQ(1, func2.getParameters().size());
    EXPECT_EQ("a", func2.getParameters()[0].name);
    EXPECT_EQ(Symbol::DataType::INT, func2.getParameters()[0].dataType);
}

TEST_F(FunctionSymbolTest,
SetParameters) {
    FunctionSymbol func("function", Symbol::DataType::VOID, {}, false);
    EXPECT_EQ(0, func.getParameters().size());

    func.setParameters(FunctionSymbol::ParameterList({ FunctionSymbol::Parameter("b", Symbol::DataType::INT) }));
    EXPECT_EQ(1, func.getParameters().size());
    EXPECT_EQ("b", func.getParameters()[0].name);
    EXPECT_EQ(Symbol::DataType::INT, func.getParameters()[0].dataType);
}

TEST_F(FunctionSymbolTest,
IsDefined) {
    FunctionSymbol func("function", Symbol::DataType::VOID, {}, false);

    EXPECT_FALSE(func.isDefined());
}

TEST_F(FunctionSymbolTest,
SetDefined) {
    FunctionSymbol func("function" , Symbol::DataType::VOID, {}, false);
    EXPECT_FALSE(func.isDefined());

    func.setDefined(true);
    EXPECT_TRUE(func.isDefined());

    func.setDefined(false);
    EXPECT_FALSE(func.isDefined());
}

class SymbolTableTest : public Test {};

TEST_F(SymbolTableTest,
DefaultInitialization) {
    SymbolTable table;

    EXPECT_EQ(0, table.getSize());
}

TEST_F(SymbolTableTest,
AddVariable) {
    SymbolTable table;
    const VariableSymbol* addedSymbol = table.addVariable("var", Symbol::DataType::INT);

    EXPECT_EQ(1, table.getSize());
    EXPECT_EQ("var", addedSymbol->getName());
    EXPECT_EQ(Symbol::Type::VARIABLE, addedSymbol->getType());
    EXPECT_EQ(Symbol::DataType::INT, addedSymbol->getDataType());
}

TEST_F(SymbolTableTest,
AddFunction) {
    SymbolTable table;
    const FunctionSymbol* addedSymbol = table.addFunction("func", Symbol::DataType::VOID, {}, true);

    EXPECT_EQ(1, table.getSize());
    EXPECT_EQ("func", addedSymbol->getName());
    EXPECT_EQ(Symbol::Type::FUNCTION, addedSymbol->getType());
    EXPECT_EQ(Symbol::DataType::VOID, addedSymbol->getReturnType());
    EXPECT_EQ(FunctionSymbol::ParameterList({}), addedSymbol->getParameters());
    EXPECT_TRUE(addedSymbol->isDefined());
}

TEST_F(SymbolTableTest,
FindSymbol) {
    SymbolTable table;
    EXPECT_EQ(nullptr, table.findSymbol("foo"));

    table.addVariable("foo", Symbol::DataType::INT);
    const Symbol* foundSymbol = table.findSymbol("foo");
    EXPECT_EQ("foo", foundSymbol->getName());
    EXPECT_EQ(Symbol::DataType::INT, static_cast<const VariableSymbol*>(foundSymbol)->getDataType());
}

TEST_F(SymbolTableTest,
CollisionOnSameNameVariable) {
    SymbolTable table;
    table.addVariable("var", Symbol::DataType::INT);

    EXPECT_EQ(1, table.getSize());
    EXPECT_EQ(nullptr, table.addVariable("var", Symbol::DataType::CHAR));
    EXPECT_EQ(1, table.getSize());
}

TEST_F(SymbolTableTest,
PostponedFunctionDefinition) {
    SymbolTable table;
    const FunctionSymbol* fooFunc = table.addFunction("foo", Symbol::DataType::VOID, {}, false);
    EXPECT_EQ(1, table.getSize());

    EXPECT_NE(nullptr, table.addFunction("foo", Symbol::DataType::VOID, {}, true));
    EXPECT_EQ(1, table.getSize());
    EXPECT_TRUE(fooFunc->isDefined());
}

TEST_F(SymbolTableTest,
PostponedFunctionDefinitionWithDifferentReturnType) {
    SymbolTable table;
    const FunctionSymbol* fooFunc = table.addFunction("foo", Symbol::DataType::VOID, {}, false);
    EXPECT_EQ(1, table.getSize());

    EXPECT_EQ(nullptr, table.addFunction("foo", Symbol::DataType::INT, {}, true));
    EXPECT_EQ(1, table.getSize());
    EXPECT_FALSE(fooFunc->isDefined());
}

TEST_F(SymbolTableTest,
PostponedFunctionDefinitionWithDifferentParameters) {
    SymbolTable table;
    const FunctionSymbol* fooFunc = table.addFunction("foo", Symbol::DataType::VOID, {}, false);
    EXPECT_EQ(1, table.getSize());

    EXPECT_EQ(nullptr, table.addFunction("foo", Symbol::DataType::VOID, { FunctionSymbol::Parameter("x", Symbol::DataType::INT) }, true));
    EXPECT_EQ(1, table.getSize());
    EXPECT_FALSE(fooFunc->isDefined());
}

TEST_F(SymbolTableTest,
FunctionRedefinition) {
    SymbolTable table;
    const FunctionSymbol* fooFunc = table.addFunction("foo", Symbol::DataType::VOID, {}, true);
    EXPECT_EQ(1, table.getSize());

    EXPECT_EQ(nullptr, table.addFunction("foo", Symbol::DataType::VOID, {}, false));
    EXPECT_EQ(1, table.getSize());
    EXPECT_TRUE(fooFunc->isDefined());
}

class ContextTest : public Test {};

TEST_F(ContextTest,
DefaultInitialization) {
    Context context;

    EXPECT_NE(nullptr, context.globalSymbolTable());
    EXPECT_EQ(nullptr, context.currentSymbolTable());
}

TEST_F(ContextTest,
NewSymbolTable) {
    Context context;
    context.newSymbolTable();
    EXPECT_NE(nullptr, context.currentSymbolTable());

    SymbolTable* table = context.currentSymbolTable();
    context.newSymbolTable();
    EXPECT_NE(table, context.currentSymbolTable());
}

TEST_F(ContextTest,
PopSymbolTable) {
    Context context;
    context.newSymbolTable();
    SymbolTable* table = context.currentSymbolTable();
    context.newSymbolTable();
    EXPECT_NE(table, context.currentSymbolTable());

    context.popSymbolTable();
    EXPECT_EQ(table, context.currentSymbolTable());

    context.popSymbolTable();
    EXPECT_EQ(nullptr, context.currentSymbolTable());
}

TEST_F(ContextTest,
FindSymbolIfOnlyOneExistsInHierarchy) {
    Context context;
    context.newSymbolTable();
    context.currentSymbolTable()->addVariable("x", Symbol::DataType::INT);
    const Symbol* foundSymbol = context.findSymbol("x");

    EXPECT_EQ(Symbol::Type::VARIABLE, foundSymbol->getType());
    EXPECT_EQ("x", foundSymbol->getName());
    EXPECT_EQ(Symbol::DataType::INT, static_cast<const VariableSymbol*>(foundSymbol)->getDataType());
}

TEST_F(ContextTest,
FindSymbolIfMoreExistsInHierarchy) {
    Context context;
    context.newSymbolTable();
    context.currentSymbolTable()->addVariable("x", Symbol::DataType::INT);
    context.newSymbolTable();
    context.currentSymbolTable()->addVariable("x", Symbol::DataType::CHAR);
    const Symbol* foundSymbol = context.findSymbol("x");

    EXPECT_EQ(Symbol::Type::VARIABLE, foundSymbol->getType());
    EXPECT_EQ("x", foundSymbol->getName());
    EXPECT_EQ(Symbol::DataType::CHAR, static_cast<const VariableSymbol*>(foundSymbol)->getDataType());
}
