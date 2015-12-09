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
		CALL,
		BUILTIN_CALL,
		BIT_AND,
		BIT_OR,
		BIT_NOT,
		NEG
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
	virtual ~Call()
	{
		for (auto& param : _parameters)
			delete param;
	}

	virtual Generates generates() override { return Generates::VALUE; }
	virtual ir::Value* generateIrValue(ir::Builder& builder) override;

private:
	Call& operator =(const Call&);

	std::vector<Expression*> _parameters;
};

class BuiltinCall : public TerminalExpression<std::string>
{
public:
	BuiltinCall(const std::string& functionName, Symbol::DataType returnDataType, const std::vector<Expression*>& parameters) :
		TerminalExpression<std::string>(Type::BUILTIN_CALL, returnDataType, functionName), _parameters(parameters) {}
	BuiltinCall(const BuiltinCall&) = delete;
	virtual ~BuiltinCall()
	{
		for (auto& param : _parameters)
			delete param;
	}

	virtual Generates generates() override { return Generates::VALUE; }
	virtual ir::Value* generateIrValue(ir::Builder& builder) override;

private:
	BuiltinCall& operator =(const BuiltinCall&);

	std::vector<Expression*> _parameters;
};

class UnaryExpression : public Expression
{
public:
	UnaryExpression(Type type, Symbol::DataType dataType, Expression* operand) : Expression(type, dataType), _operand(operand) {}
	UnaryExpression(const UnaryExpression&) = delete;
	virtual ~UnaryExpression()
	{
		delete _operand;
	}

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
	virtual ~BinaryExpression()
	{
		delete _leftOperand;
		delete _rightOperand;
	}

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
	virtual ~StatementBlock()
	{
		for (auto& stmt : _statements)
			delete stmt;
	}

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
	virtual ~AssignStatement()
	{
		delete _expression;
	}

	virtual Generates generates() override { return Generates::NOTHING; }
	virtual void generateIr(ir::Builder& builder) override;

private:
	AssignStatement& operator =(const AssignStatement&);

	VariableSymbol* _variable;
	Expression* _expression;
};

class Declaration : public ASTNode
{
public:
	Declaration(const std::string& variableName, Expression* initialization) : ASTNode(),
		_variable(nullptr), _variableName(variableName), _initialization(initialization) {}
	Declaration(const Declaration&) = delete;
	virtual ~Declaration()
	{
		delete _initialization;
	}

	virtual Generates generates() override { return Generates::NOTHING; }
	virtual void generateIr(ir::Builder& builder) override;

	void setVariableSymbol(VariableSymbol* symbol) { _variable = symbol; }
	VariableSymbol* getVariableSymbol() const { return _variable; }

	Expression* getInitialization() const { return _initialization; }
	const std::string& getVariableName() const { return _variableName; }

private:
	Declaration& operator =(const Declaration&);

	VariableSymbol* _variable;
	std::string _variableName;
	Expression* _initialization;
};

class DeclarationStatement : public Statement
{
public:
	DeclarationStatement(const std::vector<Declaration*>& declarations) : Statement(), _declarations(declarations) {}
	DeclarationStatement(const DeclarationStatement&) = delete;
	virtual ~DeclarationStatement()
	{
		for (auto& decl : _declarations)
			delete decl;
	}

	virtual Generates generates() override { return Generates::NOTHING; }
	virtual void generateIr(ir::Builder& builder) override;

private:
	DeclarationStatement& operator =(const DeclarationStatement&);

	std::vector<Declaration*> _declarations;
};

class IfStatement : public Statement
{

public:
	IfStatement(Expression* expression, StatementBlock* ifStatements, StatementBlock* elseStatements) :
		Statement(), _expression(expression), _ifStatements(ifStatements), _elseStatements(elseStatements) {}
	IfStatement(const IfStatement&) = delete;
	virtual ~IfStatement()
	{
		delete _expression;
		delete _ifStatements;
		delete _elseStatements;
	}

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
	virtual ~WhileStatement()
	{
		delete _expression;
		delete _statements;
	}

	virtual Generates generates() override { return Generates::BLOCKS; }
	virtual ir::BasicBlock* generateIrBlocks(ir::Builder& builder) override;

private:
	WhileStatement& operator =(const WhileStatement&);

	Expression* _expression;
	StatementBlock* _statements;
};

class ForIterationStatement : public ASTNode
{
public:
	ForIterationStatement(AssignStatement* assignment) : ASTNode(), _assignment(assignment), _expression(nullptr) {}
	ForIterationStatement(Expression* expression) : ASTNode(), _assignment(nullptr), _expression(expression) {}
	ForIterationStatement(const ForIterationStatement&) = delete;
	virtual ~ForIterationStatement()
	{
		delete _assignment;
		delete _expression;
	}

	bool usesAssignment() const { return (_assignment != nullptr); }
	bool usesExpression() const { return (_expression != nullptr); }

	virtual Generates generates() override { return Generates::NOTHING; }
	virtual void generateIr(ir::Builder& builder) override;

private:
	ForIterationStatement& operator =(const ForIterationStatement&);

	AssignStatement* _assignment;
	Expression* _expression;
};

class ForStatement : public Statement
{
public:
	ForStatement(AssignStatement* initialization, Expression* condition, ForIterationStatement* iteration, StatementBlock* statements) :
		Statement(), _initialization(initialization), _condition(condition), _iteration(iteration), _statements(statements) {}
	ForStatement(const ForStatement&) = delete;
	virtual ~ForStatement()
	{
		delete _initialization;
		delete _condition;
		delete _iteration;
		delete _statements;
	}

	virtual Generates generates() override { return Generates::BLOCKS; }
	virtual ir::BasicBlock* generateIrBlocks(ir::Builder& builder) override;

private:
	ForStatement& operator =(const ForStatement&);

	AssignStatement* _initialization;
	Expression* _condition;
	ForIterationStatement* _iteration;
	StatementBlock* _statements;
};

class ReturnStatement : public Statement
{
public:
	ReturnStatement() : Statement(), _expression(nullptr) {}
	ReturnStatement(Expression* expression) : Statement(), _expression(expression) {}
	ReturnStatement(const ReturnStatement&) = delete;
	virtual ~ReturnStatement()
	{
		delete _expression;
	}

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
	virtual ~CallStatement()
	{
		for (auto& param : _parameters)
			delete param;
	}

	virtual Generates generates() override { return Generates::VALUE; }
	virtual ir::Value* generateIrValue(ir::Builder& builder) override;

private:
	CallStatement& operator =(const CallStatement&);

	FunctionSymbol* _function;
	std::vector<Expression*> _parameters;
};

class BuiltinCallStatement : public Statement
{
public:
	BuiltinCallStatement(const std::string& functionName, const std::vector<Expression*>& parameters) : Statement(), _functionName(functionName), _parameters(parameters) {}
	BuiltinCallStatement(const BuiltinCallStatement&) = delete;
	virtual ~BuiltinCallStatement()
	{
		for (auto& param : _parameters)
			delete param;
	}

	virtual Generates generates() override { return Generates::VALUE; }
	virtual ir::Value* generateIrValue(ir::Builder& builder) override;

private:
	CallStatement& operator =(const CallStatement&);

	std::string _functionName;
	std::vector<Expression*> _parameters;
};

class Function : public ASTNode
{
public:
	Function(FunctionSymbol* symbol, StatementBlock* body) : ASTNode(), _symbol(symbol), _body(body) {}
	Function(const Function&) = delete;
	virtual ~Function()
	{
		delete _body;
	}

	const FunctionSymbol* getSymbol() const { return _symbol; }

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
	virtual ~Program()
	{
		for (auto& func : _functions)
			delete func;
	}

	const std::vector<Function*>& getFunctions() const { return _functions; }
	void addFunction(Function* function) { _functions.push_back(function); }
	virtual Generates generates() override { return Generates::NOTHING; }
	virtual void generateIr(ir::Builder& builder) override;

private:
	Program& operator =(const Program&);

	std::vector<Function*> _functions;
};

} // namespace frontend

#endif // FRONTEND_AST_H
