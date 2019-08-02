#include <string>
#include "GeneratorOperatorArray.h"
#include "Generator/Value/BytecodeScript.h"
#include "Generator/ComputingOperations/BytecodeNLengthOperations.h"

ska::bytecode::GenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::ARRAY_DECLARATION>::generate(OperateOn node, GenerationContext& context) {
	auto result = GenerationOutput{ InstructionPack{} };
	ApplyNOperations(context.script(), node, Command::PUSH, result);
	result.push(Instruction { Command::POP_IN_ARR, context.script().queryNextRegister(Type {}) });
	return result;
}

ska::bytecode::GenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::ARRAY_USE>::generate(OperateOn node, GenerationContext& context) {
	return InstructionPack {};
}
