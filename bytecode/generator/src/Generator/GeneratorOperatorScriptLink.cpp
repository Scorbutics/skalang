
#include "Config/LoggerConfigLang.h"
#include <string>
#include "NodeValue/AST.h"
#include "GeneratorOperatorScriptLink.h"
#include "NodeValue/ScriptAST.h"
#include "BytecodeCommand.h"

using GeneratorOperatorCurrent = ska::bytecode::GeneratorOperator<ska::Operator::SCRIPT_LINK>;

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, GeneratorOperatorCurrent);

#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, GeneratorOperatorCurrent)

ska::bytecode::ScriptGenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::SCRIPT_LINK>::generate(OperateOn node, GenerationContext& context) {
	auto importGroup = generateNext({ context, node.GetValue(), 1 });
	const auto& scriptImportedName = *importGroup.value().as<StringShared>();
	auto [importedScriptIndex, importedScript] = context.script(scriptImportedName);

	if(importedScript == nullptr && importedScriptIndex == std::numeric_limits<std::size_t>::max()) {
		throw std::runtime_error("Unknown script linked \"" + scriptImportedName + "\"");
	}

	assert(importedScriptIndex != std::numeric_limits<std::size_t>::max());
	importGroup.push(Instruction { Command::SCRIPT, context.script().queryNextRegister(), Value{ ScriptVariableRef { importedScriptIndex, context.scriptIndex() } } });
	return importGroup;
}
