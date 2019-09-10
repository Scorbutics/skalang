#include "BytecodeExecutionContext.h"

ska::bytecode::ScriptExecution* ska::bytecode::ExecutionContext::getScript(const std::string& fullScriptName, ScriptGenerationOutput& instructions) {
	if(m_container.find(fullScriptName) != m_container.end()) {
		return m_container.at(fullScriptName).get();
	}
	auto [elementIt, isInserted] = m_container.emplace(fullScriptName, std::make_unique<ScriptExecution>(fullScriptName, instructions));
	return elementIt->second.get();
}
