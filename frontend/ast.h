#ifndef FRONTEND_AST_H
#define FRONTEND_AST_H

#include <vector>

#include "frontend/symbol_table.h"

class ASTNode;
class AST
{
public:
    AST();
    AST(const AST&) = delete;
    ~AST();

private:
    AST& operator =(const AST&);

    ASTNode* _rootNode;
};

class ASTNode
{
public:
    ASTNode(const ASTNode&) = delete;
    virtual ~ASTNode();

    //virtual IRInstruction* generateIR() = 0;
protected:
    ASTNode();

private:
    ASTNode& operator =(const ASTNode&);
};

class Expression : public ASTNode
{
public:
    Expression();
    Expression(const Expression&);
    virtual ~Expression();
};

template <typename T> class TerminalExpression : public Expression
{
public:
    TerminalExpression();
    TerminalExpression(const TerminalExpression&);
    virtual ~TerminalExpression();

protected:
    T _data;
};

class IntLiteral : public TerminalExpression<int>
{
public:
    IntLiteral(int data);
    IntLiteral(const IntLiteral&);
    virtual ~IntLiteral();
};

class CharLiteral : public TerminalExpression<char>
{
public:
    CharLiteral(char data);
    CharLiteral(const IntLiteral&);
    virtual ~CharLiteral();
};

class StringLiteral : public TerminalExpression<std::string>
{
public:
    StringLiteral(const std::string& data);
    StringLiteral(const IntLiteral&);
    virtual ~StringLiteral();
};

class Variable : public TerminalExpression<Symbol*>
{
public:
    Variable(Symbol* data);
    Variable(const Variable&);
    virtual ~Variable();
};

class Call : public TerminalExpression<FunctionSymbol*>
{
public:
    Call(FunctionSymbol* function, const std::vector<Expression*>& parameters);
    Call(const Call&);
    virtual ~Call();

private:
    FunctionSymbol* _function;
    std::vector<Expression*> _parameters;
};

enum UnaryOp
{
    UNARY_OP_NOT    = 0
};

class UnaryExpression : public Expression
{
public:
    UnaryExpression(UnaryOp operation, Expression* operand);
    UnaryExpression(const UnaryExpression&);
    virtual ~UnaryExpression();

private:
    UnaryOp _operation;
    Expression* _operand;
};

enum BinaryOp
{
    BINARY_OP_PLUS          = 0,
    BINARY_OP_MINUS,
    BINARY_OP_MULTIPLY,
    BINARY_OP_DIVIDE,
    BINARY_OP_MODULO,
    BINARY_OP_LESS,
    BINARY_OP_LESS_EQUAL,
    BINARY_OP_GREATER,
    BINARY_OP_GREATER_EQUAL,
    BINARY_OP_EQUAL,
    BINARY_OP_NOT_EQUAL,
    BINARY_OP_AND,
    BINARY_OP_OR
};

class BinaryExpression : public Expression
{
public:
    BinaryExpression(BinaryOp operation, Expression* leftOperand, Expression* rightOperand);
    BinaryExpression(const BinaryExpression&);
    virtual ~BinaryExpression();

private:
    BinaryOp _operation;
    Expression* _leftOperand;
    Expression* _rightOperand;
};

class Statement : public ASTNode
{
public:
    Statement();
    Statement(const Statement&);
    virtual ~Statement();

};

class AssignStatement : public Statement
{
public:
    AssignStatement(Symbol* symbol, Expression* expression);
    AssignStatement(const AssignStatement&);
    virtual ~AssignStatement();

private:
    Symbol* _variable;
    Expression* _expression;
};

class DeclarationStatement : public Statement
{
public:
    DeclarationStatement(const std::vector<Symbol*>& variables);
    DeclarationStatement(const DeclarationStatement&);
    virtual ~DeclarationStatement();

private:
    std::vector<Symbol*> _variables;
};

class IfStatement : public Statement
{
public:
    IfStatement(Expression* expression, const std::vector<Statement*>& ifStatements, const std::vector<Statement*>& elseStatements);
    IfStatement(const IfStatement&);
    virtual ~IfStatement();

private:
    Expression* _expression;
    std::vector<Statement*> _ifStatements, _elseStatements;
};

class WhileStatement : public Statement
{
public:
    WhileStatement(Expression* expression, const std::vector<Statement*>& statements);
    WhileStatement(const WhileStatement&);
    virtual ~WhileStatement();

private:
    Expression* _expression;
    std::vector<Statement*> _statements;
};

class ReturnStatement : public Statement
{
public:
    ReturnStatement();
    ReturnStatement(Expression* expression);
    ReturnStatement(const ReturnStatement&);
    virtual ~ReturnStatement();

private:
    Expression* _expression;
};

class CallStatement : public Statement
{
public:
    CallStatement(FunctionSymbol* function, const std::vector<Expression*>& parameters);
    CallStatement(const CallStatement&);
    virtual ~CallStatement();

private:
    FunctionSymbol* _function;
    std::vector<Expression*> _parameters;
};

class Function : public ASTNode
{
public:
    Function(FunctionSymbol* symbol, const std::vector<Statement*>& body);
    Function(const Function&);
    virtual ~Function();

private:
    FunctionSymbol* _symbol;
    std::vector<Statement*> _body;
};

class Program : public ASTNode
{
public:
    Program();
    Program(const Program&);
    virtual ~Program();

    void addFunction(Function* function);

private:
    std::vector<Function*> _functions;
};

#endif
