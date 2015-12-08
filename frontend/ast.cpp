#include "frontend/ast.h"
#include "ir/instruction.h"

namespace frontend {

ir::Value* IntLiteral::generateIrValue(ir::Builder& builder)
{
	return builder.createConstantValue(_data);
}

ir::Value* CharLiteral::generateIrValue(ir::Builder& builder)
{
	return builder.createConstantValue(_data);
}

ir::Value* StringLiteral::generateIrValue(ir::Builder& builder)
{
	return builder.createConstantValue(_data);
}

ir::Value* Variable::generateIrValue(ir::Builder& /*builder*/)
{
	return _data->getIrValue();
}

ir::Value* Call::generateIrValue(ir::Builder& builder)
{
	std::vector<ir::Value*> paramValues;
	for (Expression* param : _parameters)
		paramValues.push_back(param->generateIrValue(builder));

	return builder.createCall(_data->getName(), paramValues);
}

ir::Value* BuiltinCall::generateIrValue(ir::Builder& builder)
{
	std::vector<ir::Value*> paramValues;
	for (Expression* param : _parameters)
		paramValues.push_back(param->generateIrValue(builder));

	return builder.createBuiltinCall(_data, Symbol::dataTypeToIrDataType(getDataType()), paramValues);
}

ir::Value* CallStatement::generateIrValue(ir::Builder& builder)
{
	std::vector<ir::Value*> paramValues;
	for (Expression* param : _parameters)
		paramValues.push_back(param->generateIrValue(builder));

	return builder.createCall(_function->getName(), paramValues);
}

ir::Value* BuiltinCallStatement::generateIrValue(ir::Builder& builder)
{
	std::vector<ir::Value*> paramValues;
	for (Expression* param : _parameters)
		paramValues.push_back(param->generateIrValue(builder));

	return builder.createBuiltinCall(_functionName, Symbol::dataTypeToIrDataType(Symbol::DataType::VOID), paramValues);
}

ir::Value* UnaryExpression::generateIrValue(ir::Builder& builder)
{
	ir::Value* operand = _operand->generateIrValue(builder);

	ir::Value* resultValue = nullptr;
	switch (getType())
	{
		case Expression::Type::NOT:
			resultValue = builder.createUnaryOperation<ir::NotInstruction>(operand, Symbol::dataTypeToIrDataType(getDataType()));
			break;
		case Expression::Type::TYPECAST:
			resultValue = builder.createUnaryOperation<ir::TypecastInstruction>(operand, Symbol::dataTypeToIrDataType(getDataType()));
			break;
		case Expression::Type::BIT_NOT:
			resultValue = builder.createUnaryOperation<ir::BitwiseNotInstruction>(operand, Symbol::dataTypeToIrDataType(getDataType()));
			break;
		default:
			return nullptr;
	}

	return resultValue;
}

ir::Value* BinaryExpression::generateIrValue(ir::Builder& builder)
{
	ir::Value* leftOperandValue = _leftOperand->generateIrValue(builder);
	ir::Value* rightOperandValue = _rightOperand->generateIrValue(builder);

	ir::Value* resultValue = nullptr;
	switch (getType())
	{
		case Expression::Type::ADD:
			resultValue = builder.createBinaryOperation<ir::AddInstruction>(leftOperandValue, rightOperandValue,
				Symbol::dataTypeToIrDataType(getDataType()));
			break;
		case Expression::Type::SUBTRACT:
			resultValue = builder.createBinaryOperation<ir::SubtractInstruction>(leftOperandValue, rightOperandValue,
				Symbol::dataTypeToIrDataType(getDataType()));
			break;
		case Expression::Type::MULTIPLY:
			resultValue = builder.createBinaryOperation<ir::MultiplyInstruction>(leftOperandValue, rightOperandValue,
				Symbol::dataTypeToIrDataType(getDataType()));
			break;
		case Expression::Type::DIVIDE:
			resultValue = builder.createBinaryOperation<ir::DivideInstruction>(leftOperandValue, rightOperandValue,
				Symbol::dataTypeToIrDataType(getDataType()));
			break;
		case Expression::Type::MODULO:
			resultValue = builder.createBinaryOperation<ir::ModuloInstruction>(leftOperandValue, rightOperandValue,
				Symbol::dataTypeToIrDataType(getDataType()));
			break;
		case Expression::Type::LESS:
			resultValue = builder.createBinaryOperation<ir::LessInstruction>(leftOperandValue, rightOperandValue,
				Symbol::dataTypeToIrDataType(getDataType()));
			break;
		case Expression::Type::LESS_EQUAL:
			resultValue = builder.createBinaryOperation<ir::LessEqualInstruction>(leftOperandValue, rightOperandValue,
				Symbol::dataTypeToIrDataType(getDataType()));
			break;
		case Expression::Type::GREATER:
			resultValue = builder.createBinaryOperation<ir::GreaterInstruction>(leftOperandValue, rightOperandValue,
				Symbol::dataTypeToIrDataType(getDataType()));
			break;
		case Expression::Type::GREATER_EQUAL:
			resultValue = builder.createBinaryOperation<ir::GreaterEqualInstruction>(leftOperandValue, rightOperandValue,
				Symbol::dataTypeToIrDataType(getDataType()));
			break;
		case Expression::Type::EQUAL:
			resultValue = builder.createBinaryOperation<ir::EqualInstruction>(leftOperandValue, rightOperandValue,
				Symbol::dataTypeToIrDataType(getDataType()));
			break;
		case Expression::Type::NOT_EQUAL:
			resultValue = builder.createBinaryOperation<ir::NotEqualInstruction>(leftOperandValue, rightOperandValue,
				Symbol::dataTypeToIrDataType(getDataType()));
			break;
		case Expression::Type::AND:
			resultValue = builder.createBinaryOperation<ir::AndInstruction>(leftOperandValue, rightOperandValue,
				Symbol::dataTypeToIrDataType(getDataType()));
			break;
		case Expression::Type::OR:
			resultValue = builder.createBinaryOperation<ir::OrInstruction>(leftOperandValue, rightOperandValue,
				Symbol::dataTypeToIrDataType(getDataType()));
			break;
		case Expression::Type::BIT_AND:
			resultValue = builder.createBinaryOperation<ir::BitwiseAndInstruction>(leftOperandValue, rightOperandValue,
				Symbol::dataTypeToIrDataType(getDataType()));
			break;
		case Expression::Type::BIT_OR:
			resultValue = builder.createBinaryOperation<ir::BitwiseOrInstruction>(leftOperandValue, rightOperandValue,
				Symbol::dataTypeToIrDataType(getDataType()));
			break;
		default:
			return nullptr;
	}

	return resultValue;
}

void AssignStatement::generateIr(ir::Builder& builder)
{
	ir::Value* exprValue = _expression->generateIrValue(builder);
	builder.createAssignment(_variable->getIrValue(), exprValue);
}

void DeclarationStatement::generateIr(ir::Builder& builder)
{
	for (VariableSymbol* var : _variables)
	{
		ir::Value* varValue = builder.createDeclaration(var->getName(), Symbol::dataTypeToIrDataType(var->getDataType()));
		var->setIrValue(varValue);
	}
}

ir::BasicBlock* StatementBlock::generateIrBlocks(ir::Builder& builder)
{
	ir::BasicBlock* endBlock = builder.getActiveBasicBlock();
	for (Statement* stmt : _statements)
	{
		switch (stmt->generates())
		{
			case Generates::BLOCKS:
				endBlock = stmt->generateIrBlocks(builder);
				break;
			case Generates::VALUE:
				stmt->generateIrValue(builder);
				break;
			case Generates::NOTHING:
				stmt->generateIr(builder);
				break;
		}
	}

	return endBlock;
}

ir::BasicBlock* IfStatement::generateIrBlocks(ir::Builder& builder)
{
	ir::Value* condValue = _expression->generateIrValue(builder);
	ir::BasicBlock* ifBlock = builder.createBasicBlock();
	ir::BasicBlock* elseBlock = builder.createBasicBlock();
	ir::BasicBlock* joinBlock = builder.createBasicBlock();

	builder.createConditionalJump(condValue, ifBlock, elseBlock);

	builder.addBasicBlock(ifBlock);
	builder.setActiveBasicBlock(ifBlock);
	ir::BasicBlock* endIfBlock = _ifStatements->generateIrBlocks(builder);
	builder.setActiveBasicBlock(endIfBlock);
	// Create trailing jump only when the last instruction of basic block is not return
	if (dynamic_cast<ir::ReturnInstruction*>(endIfBlock->getTerminalInstruction()) == nullptr)
		builder.createJump(joinBlock);

	builder.addBasicBlock(elseBlock);
	builder.setActiveBasicBlock(elseBlock);
	ir::BasicBlock* endElseBlock = _elseStatements->generateIrBlocks(builder);
	builder.setActiveBasicBlock(endElseBlock);
	// Create trailing jump only when the last instruction of basic block is not return
	if (dynamic_cast<ir::ReturnInstruction*>(endElseBlock->getTerminalInstruction()) == nullptr)
		builder.createJump(joinBlock);

	builder.addBasicBlock(joinBlock);
	builder.setActiveBasicBlock(joinBlock);
	return joinBlock;
}

ir::BasicBlock* WhileStatement::generateIrBlocks(ir::Builder& builder)
{
	ir::BasicBlock* condBlock = builder.createBasicBlock();
	ir::BasicBlock* bodyBlock = builder.createBasicBlock();
	ir::BasicBlock* endBlock = builder.createBasicBlock();

	builder.createJump(condBlock);

	builder.addBasicBlock(condBlock);
	builder.setActiveBasicBlock(condBlock);
	ir::Value* condValue = _expression->generateIrValue(builder);
	builder.createConditionalJump(condValue, bodyBlock, endBlock);

	builder.addBasicBlock(bodyBlock);
	builder.setActiveBasicBlock(bodyBlock);
	ir::BasicBlock* endBodyBlock = _statements->generateIrBlocks(builder);
	builder.setActiveBasicBlock(endBodyBlock);
	builder.createJump(condBlock);

	builder.addBasicBlock(endBlock);
	builder.setActiveBasicBlock(endBlock);
	return endBlock;
}

void ReturnStatement::generateIr(ir::Builder& builder)
{
	ir::Value* returnValue = nullptr;
	if (_expression != nullptr)
		returnValue = _expression->generateIrValue(builder);

	builder.createReturn(returnValue);
}

void Function::generateIr(ir::Builder& builder)
{
	std::vector<ir::Value*> irParameters;
	const FunctionSymbol::ParameterList& parameters = _symbol->getParameters();
	for (auto& param : parameters)
	{
		ir::Value* value = builder.createNamedValue(Symbol::dataTypeToIrDataType(param->getDataType()), param->getName());
		param->setIrValue(value);
		irParameters.push_back(value);
	}

	ir::Function* irFunction = builder.createFunction(_symbol->getName(), irParameters);
	if (irFunction == nullptr)
		return;

	builder.setActiveFunction(irFunction);

	// Add default return
	builder.setActiveBasicBlock(irFunction->getTerminalBasicBlock());
	ir::Value* defaultRetValue = nullptr;
	switch (_symbol->getReturnType())
	{
		case Symbol::DataType::INT:
			defaultRetValue = builder.createConstantValue(0);
			break;
		case Symbol::DataType::CHAR:
			defaultRetValue = builder.createConstantValue('\0');
			break;
		case Symbol::DataType::STRING:
			defaultRetValue = builder.createConstantValue("");
			break;
		default:
			break;
	}
	builder.createReturn(defaultRetValue);

	ir::BasicBlock* entryBlock = irFunction->getEntryBasicBlock();
	builder.setActiveBasicBlock(entryBlock);
	_body->generateIrBlocks(builder);

	// Add jump to the terminal block of the function of every single basic block with no successor
	for (auto& bb : irFunction->getBasicBlocks())
	{
		if (bb == irFunction->getTerminalBasicBlock())
			continue;

		if (bb->getTerminalInstruction() == nullptr ||
			((dynamic_cast<ir::ReturnInstruction*>(bb->getTerminalInstruction()) == nullptr) &&
			 (dynamic_cast<ir::JumpInstruction*>(bb->getTerminalInstruction()) == nullptr) &&
			 (dynamic_cast<ir::CondJumpInstruction*>(bb->getTerminalInstruction()) == nullptr)))
		{
			builder.setActiveBasicBlock(bb);
			builder.createJump(irFunction->getTerminalBasicBlock());
		}
	}
}

void Program::generateIr(ir::Builder& builder)
{
	for (Function* function : _functions)
		function->generateIr(builder);
}

} // namespace frontend
