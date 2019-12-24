#include <string>
#include "GeneratorOperatorArray.h"
#include "Generator/Value/BytecodeScriptGenerationHelper.h"
#include "Generator/ComputingOperations/BytecodeNLengthOperations.h"

ska::bytecode::InstructionOutput ska::bytecode::GeneratorOperator<ska::Operator::ARRAY_DECLARATION>::generate(OperateOn node, GenerationContext& context) {
	auto result = InstructionOutput{ };
	auto totalPushedElements = applyGenerator(ApplyNOperations<Command::PUSH, OperateOn&>, result, context, node, std::numeric_limits<std::size_t>::max());
	result.push(Instruction { Command::POP_IN_ARR, context.queryNextRegister(), Operand { static_cast<long>(totalPushedElements) } });
	return result;
}

ska::bytecode::InstructionOutput ska::bytecode::GeneratorOperator<ska::Operator::ARRAY_USE>::generate(OperateOn node, GenerationContext& context) {
	//TODO Command::ARR_ACCESS
	return {};
}
