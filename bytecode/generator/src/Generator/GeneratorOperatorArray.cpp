#include <string>
#include "GeneratorOperatorArray.h"
#include "Generator/Value/BytecodeScript.h"
#include "Generator/ComputingOperations/BytecodeNLengthOperations.h"

ska::bytecode::GenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::ARRAY_DECLARATION>::generate(OperateOn node, GenerationContext& context) {
	auto result = GenerationOutput{ };
	ApplyNOperations<Command::PUSH>(result, context.script(), node);
	result.push(Instruction { Command::POP_IN_ARR, context.script().queryNextRegister() });
	return result;
}

ska::bytecode::GenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::ARRAY_USE>::generate(OperateOn node, GenerationContext& context) {
	//TODO Command::ARR_ACCESS
	return {};
}
