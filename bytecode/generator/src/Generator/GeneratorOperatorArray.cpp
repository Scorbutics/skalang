#include <string>
#include "GeneratorOperatorArray.h"
#include "Generator/Value/BytecodeScriptGenerationHelper.h"
#include "Generator/ComputingOperations/BytecodeNLengthOperations.h"

ska::bytecode::InstructionOutput ska::bytecode::GeneratorOperator<ska::Operator::ARRAY_TYPE_DECLARATION>::generate(OperateOn node, GenerationContext& context) {
	auto result = InstructionOutput{ };
	auto totalPushedElements = applyGenerator(ApplyNOperations<Command::PUSH, decltype(node.GetArrayContent().begin())>, result, context, node.GetArrayContent().begin(), node.GetArrayContent().end());
	result.push(Instruction { Command::POP_IN_ARR, context.queryNextRegister(), Operand { static_cast<long>(totalPushedElements), OperandType::PURE} });
	return result;
}

ska::bytecode::InstructionOutput ska::bytecode::GeneratorOperator<ska::Operator::ARRAY_USE>::generate(OperateOn node, GenerationContext& context) {
	auto result = InstructionOutput{ };

	auto arrayAccessedComputing = generateNext({ context, node.GetArray() });
	auto arrayIndexComputing = generateNext({ context, node.GetArrayIndex() });

	auto arrayIndex = arrayIndexComputing.operand();
	auto arrayAccessed = arrayAccessedComputing.operand();
	result.push(std::move(arrayIndexComputing));
	result.push(std::move(arrayAccessedComputing));
	result.push(Instruction{ Command::ARR_ACCESS, context.queryNextRegister(), std::move(arrayAccessed), std::move(arrayIndex) });

	return result;
}
