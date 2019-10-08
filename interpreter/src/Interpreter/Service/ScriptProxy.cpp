#include "Config/LoggerConfigLang.h"
#include "ScriptProxy.h"
#include "Interpreter/Value/Script.h"
#include "Service/ScriptNameBuilder.h"

#include "Interpreter/InterpreterOperatorFunctionCall.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::ScriptProxy);

ska::ScriptProxy::ScriptProxy(
	ScriptBridge& binding) :
	m_binding(binding),
	m_script(binding.script()) {
		SLOG(LogLevel::Info) << "Creating script proxy : " << m_script.name();
}

ska::NodeValue ska::ScriptProxy::callFunction(Interpreter& interpreter, std::string importName, std::string functionName, std::vector<ska::NodeValue> parametersValues) {
	SLOG(LogLevel::Info) << "Looking for import \"" << importName << "\" in script \"" << m_script.name() << "\"";
	auto import = m_script.findInMemoryTree(importName);
	if (import.first == nullptr) {
		throw std::runtime_error("unable to find import \"" + importName + "\" queried in script \"" + m_script.name() + "\"");
	}
	const auto importedScriptId = import.first->nodeval<ska::ScriptVariableRef>();
	auto importedScript = m_script.useImport(importedScriptId.script);
	auto functionToCallMemory = importedScript->downMemory()(functionName);
	auto& functionToExecute = functionToCallMemory.first->nodeval<ska::ScriptVariableRef>();

	auto* stored = importedScript->getFunction(functionToExecute.variable);
	auto contextToExecute = ExecutionContext{ *importedScript, *stored->node, stored->memory };

	auto operateOnFunction = ska::Operation<ska::Operator::FUNCTION_DECLARATION>(contextToExecute);
	return ska::InterpreterOperationFunctionCallScriptWithParams(m_script, interpreter, functionToCallMemory.second, operateOnFunction, std::move(parametersValues)).asRvalue().object;
}

ska::MemoryLValue ska::ScriptProxy::accessMemory(std::string importName, std::string field) {
	SLOG(LogLevel::Info) << "Looking for import \"" << importName << "\" in script \"" << m_script.name() << "\"";
	auto found = m_script.findInMemoryTree(importName).first;
	if (found == nullptr) {
		throw std::runtime_error("unable to find import \"" + importName + "\" queried in script \"" + m_script.name() + "\"");
	}
	auto& emId = found->nodeval<ska::ScriptVariableRef>();
	auto em = m_script.useImport(emId.script);
	return em->downMemory()(field);
}
