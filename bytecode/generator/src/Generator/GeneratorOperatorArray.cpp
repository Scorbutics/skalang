#include <string>
#include "GeneratorOperatorArray.h"
#include "Generator/Value/BytecodeScriptGenerationHelper.h"
#include "Generator/ComputingOperations/BytecodeNLengthOperations.h"

ska::bytecode::InstructionOutput ska::bytecode::GeneratorOperator<ska::Operator::ARRAY_DECLARATION>::generate(OperateOn node, GenerationContext& context) {
	auto result = InstructionOutput{ };
	auto totalPushedElements = ApplyNOperations<Command::PUSH>(result, context, node);
	result.push(Instruction { Command::POP_IN_ARR, context.queryNextRegister(), Operand { static_cast<long>(totalPushedElements) } });
	return result;
}

ska::bytecode::InstructionOutput ska::bytecode::GeneratorOperator<ska::Operator::ARRAY_USE>::generate(OperateOn node, GenerationContext& context) {
	//TODO Command::ARR_ACCESS
	return {};
}
