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

ir::Value* CallStatement::generateIrValue(ir::Builder& builder)
{
	std::vector<ir::Value*> paramValues;
	for (Expression* param : _parameters)
		paramValues.push_back(param->generateIrValue(builder));

	return builder.createCall(_function->getName(), paramValues);
}

ir::Value* UnaryExpression::generateIrValue(ir::Builder& builder)
{
	ir::Value* operand = _operand->generateIrValue(builder);

	ir::Value* resultValue = nullptr;
	switch (_operation)
	{
		case UNARY_OP_NOT:
			resultValue = builder.createUnaryOperation<ir::NotInstruction>(operand);
			break;
	}

	return resultValue;
}

ir::Value* BinaryExpression::generateIrValue(ir::Builder& builder)
{
	ir::Value* leftOperandValue = _leftOperand->generateIrValue(builder);
	ir::Value* rightOperandValue = _rightOperand->generateIrValue(builder);

	ir::Value* resultValue = nullptr;
	switch (_operation)
	{
		case BINARY_OP_PLUS:
			resultValue = builder.createBinaryOperation<ir::AddInstruction>(leftOperandValue, rightOperandValue);
			break;
		case BINARY_OP_MINUS:
			resultValue = builder.createBinaryOperation<ir::SubtractInstruction>(leftOperandValue, rightOperandValue);
			break;
		case BINARY_OP_MULTIPLY:
			resultValue = builder.createBinaryOperation<ir::MultiplyInstruction>(leftOperandValue, rightOperandValue);
			break;
		case BINARY_OP_DIVIDE:
			resultValue = builder.createBinaryOperation<ir::DivideInstruction>(leftOperandValue, rightOperandValue);
			break;
		case BINARY_OP_MODULO:
			resultValue = builder.createBinaryOperation<ir::ModuloInstruction>(leftOperandValue, rightOperandValue);
			break;
		case BINARY_OP_LESS:
			resultValue = builder.createBinaryOperation<ir::LessInstruction>(leftOperandValue, rightOperandValue);
			break;
		case BINARY_OP_LESS_EQUAL:
			resultValue = builder.createBinaryOperation<ir::LessEqualInstruction>(leftOperandValue, rightOperandValue);
			break;
		case BINARY_OP_GREATER:
			resultValue = builder.createBinaryOperation<ir::GreaterInstruction>(leftOperandValue, rightOperandValue);
			break;
		case BINARY_OP_GREATER_EQUAL:
			resultValue = builder.createBinaryOperation<ir::GreaterEqualInstruction>(leftOperandValue, rightOperandValue);
			break;
		case BINARY_OP_EQUAL:
			resultValue = builder.createBinaryOperation<ir::EqualInstruction>(leftOperandValue, rightOperandValue);
			break;
		case BINARY_OP_NOT_EQUAL:
			resultValue = builder.createBinaryOperation<ir::NotEqualInstruction>(leftOperandValue, rightOperandValue);
			break;
		case BINARY_OP_AND:
			resultValue = builder.createBinaryOperation<ir::AndInstruction>(leftOperandValue, rightOperandValue);
			break;
		case BINARY_OP_OR:
			resultValue = builder.createBinaryOperation<ir::OrInstruction>(leftOperandValue, rightOperandValue);
			break;
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
