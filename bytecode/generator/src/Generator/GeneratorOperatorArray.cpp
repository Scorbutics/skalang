#include <string>
#include "GeneratorOperatorArray.h"
#include "Generator/Value/BytecodeScript.h"
#include "Generator/ComputingOperations/BytecodeNLengthOperations.h"

ska::bytecode::ScriptGenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::ARRAY_DECLARATION>::generate(OperateOn node, GenerationContext& context) {
	auto result = ScriptGenerationOutput{ };
	auto totalPushedElements = ApplyNOperations<Command::PUSH>(result, context, node);
	result.push(Instruction { Command::POP_IN_ARR, context.script().queryNextRegister(), Value { static_cast<long>(totalPushedElements) } });
	return result;
}

ska::bytecode::ScriptGenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::ARRAY_USE>::generate(OperateOn node, GenerationContext& context) {
	//TODO Command::ARR_ACCESS
	return {};
}
