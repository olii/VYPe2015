%{
#include <vector>

#include "frontend/ast.h"
#include "frontend/context.h"

extern int yylex();
extern void yyerror(const char* e);

extern Program program;
extern Context context;
%}

%define parse.error verbose // Better error reporting
%debug

%union
{
    Function* function;
    FunctionSymbol::ParameterList* functionParameters;
    std::vector<Statement*>* statements;
    Statement* statement;
    std::vector<Expression*>* expressions;
    Expression* expression;
    std::vector<std::string>* strList;
    std::string* strValue;
    char charValue;
    int intValue;
}


%token COMMA SEMICOLON ASSIGN LEFT_PAREN RIGHT_PAREN LEFT_CBRACE RIGHT_CBRACE PLUS MINUS
    MULTIPLY DIVIDE MODULO NOT LESS LESS_EQUAL GREATER GREATER_EQUAL EQUAL NOT_EQUAL AND OR
    IF ELSE WHILE RETURN VOID
%token <intValue> INT_LIT
%token <charValue> CHAR_LIT
%token <strValue> STRING_LIT ID TYPE

%type <function> func_impl
%type <functionParameters> decl_param_list impl_param_list
%type <statements> stmts stmt_list
%type <statement> stmt assign_stmt decl_stmt if_stmt while_stmt return_stmt call_stmt empty_stmt
%type <expressions> exprs expr_list
%type <expression> expr
%type <strList> decl_id_list

%left OR
%left AND
%left EQUAL NOT_EQUAL
%left LESS LESS_EQUAL GREATER GREATER_EQUAL
%left PLUS MINUS
%left MULTIPLY DIVIDE MODULO
%right NOT
%nonassoc LEFT_PAREN RIGHT_PAREN

%start program

%%

program :   body
        |   %empty
        ;

body    :   body func_decl
        |   body func_impl  { program.addFunction($2); }
        |   func_decl
        |   func_impl       { program.addFunction($1); }
        ;

func_decl   :   TYPE ID LEFT_PAREN VOID RIGHT_PAREN SEMICOLON
            |   TYPE ID LEFT_PAREN decl_param_list RIGHT_PAREN SEMICOLON
            |   VOID ID LEFT_PAREN VOID RIGHT_PAREN SEMICOLON
            |   VOID ID LEFT_PAREN decl_param_list RIGHT_PAREN SEMICOLON
            ;

func_impl   :   TYPE ID LEFT_PAREN VOID RIGHT_PAREN                                             { context.newSymbolTable(); }
                                                    LEFT_CBRACE stmts RIGHT_CBRACE              {
                                                                                                    // We can look for functions only in global space
                                                                                                    SymbolTable* globalSymbolTable = context.globalSymbolTable();
                                                                                                    FunctionSymbol* funcSymbol;
                                                                                                    if ((funcSymbol = globalSymbolTable->addFunction(*$2, Symbol::stringToDataType(*$1), {}, true)) == nullptr)
                                                                                                        YYACCEPT;

                                                                                                    $$ = new Function(funcSymbol, *$8);
                                                                                                    context.popSymbolTable();
                                                                                                }
            |   TYPE ID LEFT_PAREN impl_param_list RIGHT_PAREN                                  { context.newSymbolTable(); }
                                                                LEFT_CBRACE stmts RIGHT_CBRACE  {
                                                                                                    // We can look for functions only in global space
                                                                                                    SymbolTable* globalSymbolTable = context.globalSymbolTable();
                                                                                                    FunctionSymbol* funcSymbol;
                                                                                                    if ((funcSymbol = globalSymbolTable->addFunction(*$2, Symbol::stringToDataType(*$1), *$4, true)) == nullptr)
                                                                                                        YYACCEPT;

                                                                                                    $$ = new Function(funcSymbol, *$8);
                                                                                                    context.popSymbolTable();
                                                                                                }
            |   VOID ID LEFT_PAREN VOID RIGHT_PAREN                                             { context.newSymbolTable(); }
                                                    LEFT_CBRACE stmts RIGHT_CBRACE              {
                                                                                                    // We can look for functions only in global space
                                                                                                    SymbolTable* globalSymbolTable = context.globalSymbolTable();
                                                                                                    FunctionSymbol* funcSymbol;
                                                                                                    if ((funcSymbol = globalSymbolTable->addFunction(*$2, Symbol::VOID, {}, true)) == nullptr)
                                                                                                        YYACCEPT;

                                                                                                    $$ = new Function(funcSymbol, *$8);
                                                                                                    context.popSymbolTable();
                                                                                                }
            |   VOID ID LEFT_PAREN impl_param_list RIGHT_PAREN                                  { context.newSymbolTable(); }
                                                               LEFT_CBRACE stmts RIGHT_CBRACE   {
                                                                                                    // We can look for functions only in global space
                                                                                                    SymbolTable* globalSymbolTable = context.globalSymbolTable();
                                                                                                    FunctionSymbol* funcSymbol;
                                                                                                    if ((funcSymbol = globalSymbolTable->addFunction(*$2, Symbol::VOID, *$4, true)) == nullptr)
                                                                                                        YYACCEPT;

                                                                                                    $$ = new Function(funcSymbol, *$8);
                                                                                                    context.popSymbolTable();
                                                                                                }
            ;

decl_param_list :   decl_param_list COMMA TYPE      { $$->push_back(new FunctionSymbol::Parameter(Symbol::stringToDataType({*$3}), "")); }
                |   TYPE                            { $$ = new FunctionSymbol::ParameterList( { new FunctionSymbol::Parameter(Symbol::stringToDataType({*$1}), "") } ); }
                ;

impl_param_list :   impl_param_list COMMA TYPE ID   { $$->push_back(new FunctionSymbol::Parameter(Symbol::stringToDataType({*$3}), *$4)); }
                |   TYPE ID                         { $$ = new FunctionSymbol::ParameterList( { new FunctionSymbol::Parameter(Symbol::stringToDataType({*$1}), *$2) } ); }
                ;

stmts   :   stmt_list   { $$ = $1; }
        |   %empty      { $$ = new std::vector<Statement*>(); }
        ;

stmt    :   assign_stmt { $$ = $1; }
        |   decl_stmt   { $$ = $1; }
        |   if_stmt     { $$ = $1; }
        |   while_stmt  { $$ = $1; }
        |   return_stmt { $$ = $1; }
        |   call_stmt   { $$ = $1; }
        |   empty_stmt  { $$ = $1; }
        ;

stmt_list   :   stmt_list stmt              { $$->push_back($2); }
            |   stmt                        { $$ = new std::vector<Statement*>({$1}); }
            ;

assign_stmt :   ID ASSIGN expr SEMICOLON    {
                                                Symbol* symbol = context.findSymbol(*$1);
                                                if (symbol == nullptr || symbol->getType() != Symbol::VARIABLE)
                                                    YYACCEPT;

                                                $$ = new AssignStatement(symbol, $3);
                                            }
            ;

decl_stmt   :   TYPE decl_id_list SEMICOLON {
                                                std::vector<Symbol*> variables;
                                                for (auto& varName : *$2)
                                                {
                                                    Symbol* symbol = nullptr;
                                                    if ((symbol = context.findSymbol(varName)) != nullptr)
                                                    {
                                                        // We cannot shadow functions
                                                        if (symbol->getType() == Symbol::FUNCTION)
                                                            YYACCEPT;
                                                    }

                                                    // Adding can fail if there is symbol in the same block, we cannot shadow in the same block
                                                    if ((symbol = context.currentSymbolTable()->addVariable(varName, Symbol::stringToDataType(*$1))) == nullptr)
                                                        YYACCEPT;

                                                    variables.push_back(symbol);
                                                }

                                                $$ = new DeclarationStatement(variables);
                                            }
            ;

decl_id_list    :   decl_id_list COMMA ID   { $$->push_back(*$3); }
                |   ID                      { $$ = new std::vector<std::string>({*$1}); }
                ;

if_stmt :   IF LEFT_PAREN expr RIGHT_PAREN          { context.newSymbolTable(); }
                LEFT_CBRACE stmts RIGHT_CBRACE ELSE {
                                                        context.popSymbolTable();
                                                        context.newSymbolTable();
                                                    }
                LEFT_CBRACE stmts RIGHT_CBRACE      {
                                                        $$ = new IfStatement($3, *$7, *$12);
                                                        context.popSymbolTable();
                                                    }

        ;

while_stmt  :   WHILE LEFT_PAREN expr RIGHT_PAREN   { context.newSymbolTable(); }
                    LEFT_CBRACE stmts RIGHT_CBRACE  {
                                                        $$ = new WhileStatement($3, *$7);
                                                        context.popSymbolTable();
                                                    }
            ;

return_stmt :   RETURN expr SEMICOLON   { $$ = new ReturnStatement($2); }
            |   RETURN SEMICOLON        { $$ = new ReturnStatement(); }
            ;

call_stmt   :   ID LEFT_PAREN exprs RIGHT_PAREN SEMICOLON   {
                                                                // We can look for functions only in global space
                                                                Symbol* symbol = nullptr;
                                                                if ((symbol = context.globalSymbolTable()->findSymbol(*$1)) == nullptr)
                                                                    YYACCEPT;

                                                                // Symbol needs to represent function
                                                                if (symbol->getType() != Symbol::FUNCTION)
                                                                    YYACCEPT;

                                                                // Check whether number of parameters match
                                                                FunctionSymbol* func = static_cast<FunctionSymbol*>(symbol);
                                                                if (func->getParameters().size() != $3->size())
                                                                    YYACCEPT;

                                                                $$ = new CallStatement(func, *$3);
                                                            }
            ;

empty_stmt  :   SEMICOLON { $$ = nullptr; }
            ;

exprs       :   expr_list { $$ = $1; }
            |   %empty { $$ = nullptr; }
            ;

expr_list   :   expr_list COMMA expr    { $$->push_back($3); }
            |   expr                    { $$ = new std::vector<Expression*>({$1}); }
            ;

expr    :   expr PLUS expr { $$ = new BinaryExpression(BINARY_OP_PLUS, $1, $3); }
        |   expr MINUS expr { $$ = new BinaryExpression(BINARY_OP_MINUS, $1, $3); }
        |   expr MULTIPLY expr { $$ = new BinaryExpression(BINARY_OP_MULTIPLY, $1, $3); }
        |   expr DIVIDE expr { $$ = new BinaryExpression(BINARY_OP_DIVIDE, $1, $3); }
        |   expr MODULO expr { $$ = new BinaryExpression(BINARY_OP_MODULO, $1, $3); }
        |   expr LESS expr { $$ = new BinaryExpression(BINARY_OP_LESS, $1, $3); }
        |   expr LESS_EQUAL expr { $$ = new BinaryExpression(BINARY_OP_LESS_EQUAL, $1, $3); }
        |   expr GREATER expr { $$ = new BinaryExpression(BINARY_OP_GREATER, $1, $3); }
        |   expr GREATER_EQUAL expr { $$ = new BinaryExpression(BINARY_OP_GREATER_EQUAL, $1, $3); }
        |   expr EQUAL expr { $$ = new BinaryExpression(BINARY_OP_EQUAL, $1, $3); }
        |   expr NOT_EQUAL expr { $$ = new BinaryExpression(BINARY_OP_NOT_EQUAL, $1, $3); }
        |   expr AND expr { $$ = new BinaryExpression(BINARY_OP_AND, $1, $3); }
        |   expr OR expr { $$ = new BinaryExpression(BINARY_OP_OR, $1, $3); }
        |   NOT expr { $$ = new UnaryExpression(UNARY_OP_NOT, $2); }
        |   LEFT_PAREN expr RIGHT_PAREN { $$ = $2; }
        |   ID LEFT_PAREN exprs RIGHT_PAREN
                {
                    // We can look for functions only in global space
                    Symbol* symbol = nullptr;
                    if ((symbol = context.globalSymbolTable()->findSymbol(*$1)) == nullptr)
                        YYACCEPT;

                    // Symbol needs to represent function
                    if (symbol->getType() != Symbol::FUNCTION)
                        YYACCEPT;

                    // Check whether number of parameters match
                    FunctionSymbol* func = static_cast<FunctionSymbol*>(symbol);
                    if (func->getParameters().size() != $3->size())
                        YYACCEPT;

                    $$ = new Call(func, *$3);
                }
        |   ID  {
                    // Find the symbol that represents the variable
                    Symbol* symbol = context.findSymbol(*$1);
                    if (symbol == nullptr || symbol->getType() != Symbol::VARIABLE)
                        YYACCEPT;

                    $$ = new Variable(symbol);
                }
        |   INT_LIT { $$ = new IntLiteral($1); }
        |   CHAR_LIT { $$ = new CharLiteral($1); }
        |   STRING_LIT { $$ = new StringLiteral(*$1); }
        ;

%%
