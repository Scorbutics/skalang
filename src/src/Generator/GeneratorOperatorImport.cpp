#include <string>
#include "NodeValue/AST.h"
#include "GeneratorOperatorImport.h"
#include "Interpreter/Value/Script.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScript.h"

ska::bytecode::GenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::IMPORT>::generate(OperateOn node, GenerationContext& context) {
	auto importGroup = m_generator.generate({ context.script(), node.GetScriptPathNode() });
	importGroup.push(Instruction { Command::SCRIPT, context.script().queryNextRegister(), importGroup.value() });
	return importGroup;
}
