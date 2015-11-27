#ifndef FRONTEND_AST_H
#define FRONTEND_AST_H

#include <vector>

#include "frontend/symbol_table.h"
#include "ir/builder.h"
#include "ir/value.h"

namespace frontend {

enum class Generates
{
	BLOCKS,
	VALUE,
	NOTHING
};

class ASTNode
{
public:
	ASTNode(const ASTNode&) = delete;
	virtual ~ASTNode() {}

	virtual Generates generates() = 0;
	virtual void generateIr(ir::Builder& /*builder*/) { }
	virtual ir::Value* generateIrValue(ir::Builder& /*builder*/) { return nullptr; }
	virtual ir::BasicBlock* generateIrBlocks(ir::Builder& /*builder*/) { return nullptr; }
protected:
	ASTNode() {}

private:
	ASTNode& operator =(const ASTNode&);
};

class Expression : public ASTNode
{
public:
	enum class Type
	{
		ADD,
		SUBTRACT,
		MULTIPLY,
		DIVIDE,
		MODULO,
		LESS,
		LESS_EQUAL,
		GREATER,
		GREATER_EQUAL,
		EQUAL,
		NOT_EQUAL,
		AND,
		OR,
		NOT,
		TYPECAST,
		INT,
		STRING,
		CHAR,
		VARIABLE,
		CALL
	};

	Expression(const Expression&) = delete;
	virtual ~Expression() {}

	Type getType() const { return _type; }
	Symbol::DataType getDataType() const { return _dataType; }

protected:
	Expression(Type type, Symbol::DataType dataType) : _type(type), _dataType(dataType) {}

private:
	Expression& operator =(const Expression&);

	Type _type;
	Symbol::DataType _dataType;
};

template <typename T> class TerminalExpression : public Expression
{
public:
	TerminalExpression(const TerminalExpression&) = delete;
	virtual ~TerminalExpression() {}

protected:
	TerminalExpression(Type type, Symbol::DataType dataType, const T& data) : Expression(type, dataType), _data(data) {}

	T _data;

private:
	TerminalExpression<T>& operator =(const TerminalExpression<T>&);
};

class IntLiteral : public TerminalExpression<int>
{
public:
	IntLiteral(int data) : TerminalExpression<int>(Type::INT, Symbol::DataType::INT, data) {}
	IntLiteral(const IntLiteral&) = delete;
	virtual ~IntLiteral() {}

	virtual Generates generates() override { return Generates::VALUE; }
	virtual ir::Value* generateIrValue(ir::Builder& builder) override;

private:
	IntLiteral& operator =(const IntLiteral&);
};

class CharLiteral : public TerminalExpression<char>
{
public:
	CharLiteral(char data) : TerminalExpression<char>(Type::CHAR, Symbol::DataType::CHAR, data) {}
	CharLiteral(const IntLiteral&) = delete;
	virtual ~CharLiteral() {}

	virtual Generates generates() override { return Generates::VALUE; }
	virtual ir::Value* generateIrValue(ir::Builder& builder) override;

private:
	CharLiteral& operator =(const CharLiteral&);
};

class StringLiteral : public TerminalExpression<std::string>
{
public:
	StringLiteral(const std::string& data) : TerminalExpression<std::string>(Type::STRING, Symbol::DataType::STRING, data) {}
	StringLiteral(const IntLiteral&) = delete;
	virtual ~StringLiteral() {}

	virtual Generates generates() override { return Generates::VALUE; }
	virtual ir::Value* generateIrValue(ir::Builder& builder) override;

private:
	StringLiteral& operator =(const StringLiteral&);
};

class Variable : public TerminalExpression<VariableSymbol*>
{
public:
	Variable(VariableSymbol* data) : TerminalExpression<VariableSymbol*>(Type::VARIABLE, data->getDataType(), data) {}
	Variable(const Variable&) = delete;
	virtual ~Variable() {}

	virtual Generates generates() override { return Generates::VALUE; }
	virtual ir::Value* generateIrValue(ir::Builder& builder) override;

private:
	Variable& operator =(const Variable&);
};

class Call : public TerminalExpression<FunctionSymbol*>
{
public:
	Call(FunctionSymbol* function, const std::vector<Expression*>& parameters) :
		TerminalExpression<FunctionSymbol*>(Type::CALL, function->getReturnType(), function), _parameters(parameters) {}
	Call(const Call&) = delete;
	virtual ~Call() {}

	virtual Generates generates() override { return Generates::VALUE; }
	virtual ir::Value* generateIrValue(ir::Builder& builder) override;

private:
	Call& operator =(const Call&);

	std::vector<Expression*> _parameters;
};

class UnaryExpression : public Expression
{
public:
	UnaryExpression(Type type, Symbol::DataType dataType, Expression* operand) : Expression(type, dataType), _operand(operand) {}
	UnaryExpression(const UnaryExpression&) = delete;
	virtual ~UnaryExpression() {}

	virtual Generates generates() override { return Generates::VALUE; }
	virtual ir::Value* generateIrValue(ir::Builder& builder) override;

private:
	UnaryExpression& operator =(const UnaryExpression&);

	Expression* _operand;
};

class BinaryExpression : public Expression
{
public:
	BinaryExpression(Type type, Symbol::DataType dataType, Expression* leftOperand, Expression* rightOperand) :
		Expression(type, dataType), _leftOperand(leftOperand), _rightOperand(rightOperand) {}
	BinaryExpression(const BinaryExpression&) = delete;
	virtual ~BinaryExpression() {}

	virtual Generates generates() override { return Generates::VALUE; }
	virtual ir::Value* generateIrValue(ir::Builder& builder) override;

private:
	BinaryExpression& operator =(const BinaryExpression&);

	Expression* _leftOperand;
	Expression* _rightOperand;
};

class Statement : public ASTNode
{
public:
	Statement(const Statement&) = delete;
	virtual ~Statement() {}

protected:
	Statement() {}

private:
	Statement& operator =(const Statement&);
};

class StatementBlock : public ASTNode
{
public:
	StatementBlock() {}
	StatementBlock(const StatementBlock&) = delete;
	virtual ~StatementBlock() {}

	void addStatement(Statement* statement) { _statements.push_back(statement); }

	virtual Generates generates() override { return Generates::BLOCKS; }
	virtual ir::BasicBlock* generateIrBlocks(ir::Builder& builder) override;

private:
	StatementBlock& operator =(const StatementBlock&);

	std::vector<Statement*> _statements;
};

class AssignStatement : public Statement
{
public:
	AssignStatement(VariableSymbol* variable, Expression* expression) : Statement(), _variable(variable), _expression(expression) {}
	AssignStatement(const AssignStatement&) = delete;
	virtual ~AssignStatement() {}

	virtual Generates generates() override { return Generates::NOTHING; }
	virtual void generateIr(ir::Builder& builder) override;

private:
	AssignStatement& operator =(const AssignStatement&);

	VariableSymbol* _variable;
	Expression* _expression;
};

class DeclarationStatement : public Statement
{
public:
	DeclarationStatement(const std::vector<VariableSymbol*>& variables) : Statement(), _variables(variables) {}
	DeclarationStatement(const DeclarationStatement&) = delete;
	virtual ~DeclarationStatement() {}

	virtual Generates generates() override { return Generates::NOTHING; }
	virtual void generateIr(ir::Builder& builder) override;

private:
	DeclarationStatement& operator =(const DeclarationStatement&);

	std::vector<VariableSymbol*> _variables;
};

class IfStatement : public Statement
{
public:
	IfStatement(Expression* expression, StatementBlock* ifStatements, StatementBlock* elseStatements) :
		Statement(), _expression(expression), _ifStatements(ifStatements), _elseStatements(elseStatements) {}
	IfStatement(const IfStatement&) = delete;
	virtual ~IfStatement() {}

	virtual Generates generates() override { return Generates::BLOCKS; }
	virtual ir::BasicBlock* generateIrBlocks(ir::Builder& builder) override;

private:
	IfStatement& operator =(const IfStatement&);

	Expression* _expression;
	StatementBlock* _ifStatements;
	StatementBlock* _elseStatements;
};

class WhileStatement : public Statement
{
public:
	WhileStatement(Expression* expression, StatementBlock* statements) :
		Statement(), _expression(expression), _statements(statements) {}
	WhileStatement(const WhileStatement&) = delete;
	virtual ~WhileStatement() {}

	virtual Generates generates() override { return Generates::BLOCKS; }
	virtual ir::BasicBlock* generateIrBlocks(ir::Builder& builder) override;

private:
	WhileStatement& operator =(const WhileStatement&);

	Expression* _expression;
	StatementBlock* _statements;
};

class ReturnStatement : public Statement
{
public:
	ReturnStatement() : Statement(), _expression(nullptr) {}
	ReturnStatement(Expression* expression) : Statement(), _expression(expression) {}
	ReturnStatement(const ReturnStatement&) = delete;
	virtual ~ReturnStatement() {}

	virtual Generates generates() override { return Generates::NOTHING; }
	virtual void generateIr(ir::Builder& builder) override;

private:
	ReturnStatement& operator =(const ReturnStatement&);

	Expression* _expression;
};

class CallStatement : public Statement
{
public:
	CallStatement(FunctionSymbol* function, const std::vector<Expression*>& parameters) : Statement(), _function(function), _parameters(parameters) {}
	CallStatement(const CallStatement&) = delete;
	virtual ~CallStatement() {}

	virtual Generates generates() override { return Generates::VALUE; }
	virtual ir::Value* generateIrValue(ir::Builder& builder) override;

private:
	CallStatement& operator =(const CallStatement&);

	FunctionSymbol* _function;
	std::vector<Expression*> _parameters;
};

class Function : public ASTNode
{
public:
	Function(FunctionSymbol* symbol, StatementBlock* body) : ASTNode(), _symbol(symbol), _body(body) {}
	Function(const Function&) = delete;
	virtual ~Function() {}

	virtual Generates generates() override { return Generates::NOTHING; }
	virtual void generateIr(ir::Builder& builder) override;

private:
	Function& operator =(const Function&);

	FunctionSymbol* _symbol;
	StatementBlock* _body;
};

class Program : public ASTNode
{
public:
	Program() : _functions() {}
	Program(const Program&) = delete;
	virtual ~Program() {}

	void addFunction(Function* function) { _functions.push_back(function); }
	virtual Generates generates() override { return Generates::NOTHING; }
	virtual void generateIr(ir::Builder& builder) override;

private:
	Program& operator =(const Program&);

	std::vector<Function*> _functions;
};

} // namespace frontend

#endif // FRONTEND_AST_H
