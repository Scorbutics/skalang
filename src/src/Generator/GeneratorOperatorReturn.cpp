#include <string>
#include "NodeValue/AST.h"
#include "GeneratorOperatorReturn.h"
#include "Interpreter/Value/Script.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScript.h"

ska::bytecode::GenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::RETURN>::generate(OperateOn node, GenerationContext& context) {
	return Instruction {Command::END, context.script().queryVariableOrValue(node.GetValue())};
}
