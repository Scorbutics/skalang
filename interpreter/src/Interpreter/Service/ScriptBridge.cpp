#include "Config/LoggerConfigLang.h"
#include "ScriptBridge.h"
#include "Interpreter/Value/Script.h"
#include "Service/ScriptNameBuilder.h"

#include "Interpreter/InterpreterOperatorFunctionCall.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::ScriptBridge);

ska::ScriptBridge::ScriptBridge(
	ScriptCache& cache,
	std::string scriptName,
	TypeBuilder& typeBuilder,
	SymbolTableUpdater& symbolTypeUpdater,
	const ReservedKeywordsPool& reserved) :
	ScriptBinding(cache.astCache, scriptName, typeBuilder, symbolTypeUpdater, reserved),
	m_name(ScriptNameDeduce("", "bind:" + scriptName)),
	m_script(cache, m_name, std::vector<Token>{}),
	m_cache(cache) {
}

void ska::ScriptBridge::buildFunctions() {
	ScriptBinding::buildFunctions(m_script);
}

void ska::ScriptBridge::import(StatementParser& parser, Interpreter& interpreter, std::vector<std::pair<std::string, std::string>> imports) {
	auto& importBlock = ScriptBinding::import(parser, std::move(imports));
	interpreter.interpret({ m_script, importBlock});
}

ska::NodeValue ska::ScriptBridge::callFunction(Interpreter& interpreter, std::string importName, std::string functionName, std::vector<ska::NodeValue> parametersValues) {
	auto import = m_script.findInMemoryTree(importName);
	const auto importedScriptId = import.first->nodeval<ska::ScriptVariableRef>();
	auto importedScript = m_script.useImport(importedScriptId.script);
	auto functionToCallMemory = importedScript->downMemory()(functionName);
	auto& functionToExecute = functionToCallMemory.first->nodeval<ska::ScriptVariableRef>();

	auto* stored = importedScript->getFunction(functionToExecute.variable);
	auto contextToExecute = ExecutionContext{ *importedScript, *stored->node, stored->memory };

	auto operateOnFunction = ska::Operation<ska::Operator::FUNCTION_DECLARATION>(contextToExecute);
	return ska::InterpreterOperationFunctionCallScriptWithParams(m_script, interpreter, functionToCallMemory.second, operateOnFunction, std::move(parametersValues)).asRvalue().object;
}

ska::MemoryLValue ska::ScriptBridge::accessMemory(std::string importName, std::string field) {
	auto& emId = m_script.findInMemoryTree(importName).first->nodeval<ska::ScriptVariableRef>();
	auto em = m_script.useImport(emId.script);
	return em->downMemory()(field);
}
