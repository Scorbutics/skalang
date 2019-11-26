#include "Config/LoggerConfigLang.h"
#include <string>
#include "NodeValue/AST.h"
#include "GeneratorOperatorImport.h"
#include "NodeValue/ScriptAST.h"
#include "BytecodeCommand.h"

using GeneratorOperatorCurrent = ska::bytecode::GeneratorOperator<ska::Operator::IMPORT>;

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, GeneratorOperatorCurrent);

#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, GeneratorOperatorCurrent)

ska::bytecode::InstructionOutput ska::bytecode::GeneratorOperator<ska::Operator::IMPORT>::generate(OperateOn node, GenerationContext& context) {
	auto importGroup = generateNext({ context, node.GetScriptPathNode(), 1 });
	const auto& scriptImportedName = *importGroup.operand().as<StringShared>();
	auto [importedScriptIndex, importedScript] = context.script(scriptImportedName);

	if(importedScript == nullptr && importedScriptIndex == std::numeric_limits<std::size_t>::max()) {
		LOG_DEBUG << "%10cUnknown script " << scriptImportedName << ", generating it...";
		auto scriptImported = context.script().program().useImport(scriptImportedName);
		assert(scriptImported != nullptr);
		auto scriptContext = GenerationContext{context, ScriptGenerationHelper{context.totalScripts() + 1, *scriptImported }};
		generateNext(scriptContext);
		LOG_DEBUG << "%10cGenerated script.";
		std::tie(importedScriptIndex, importedScript) = context.script(scriptImportedName);
	}

	assert(importedScriptIndex != std::numeric_limits<std::size_t>::max());
	importGroup.push(Instruction { Command::SCRIPT, context.script().queryNextRegister(), Operand { ScriptVariableRef { importedScriptIndex, context.scriptIndex() } } });
	return importGroup;
}
