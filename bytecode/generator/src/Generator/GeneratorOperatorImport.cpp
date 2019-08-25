#include <string>
#include "NodeValue/AST.h"
#include "GeneratorOperatorImport.h"
#include "NodeValue/ScriptAST.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScript.h"

ska::bytecode::GenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::IMPORT>::generate(OperateOn node, GenerationContext& context) {
	auto importGroup = generateNext({ context.script(), node.GetScriptPathNode(), context.scope() + 1 });
	importGroup.push(Instruction { Command::SCRIPT, context.script().queryNextRegister(), importGroup.value() });
	return importGroup;
}
