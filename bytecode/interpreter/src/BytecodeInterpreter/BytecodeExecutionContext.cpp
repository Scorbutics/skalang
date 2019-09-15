#include "Config/LoggerConfigLang.h"
#include "Generator/BytecodeGenerator.h"
#include "Generator/BytecodeGenerationContext.h"
#include "BytecodeExecutionContext.h"
#include "Generator/Value/BytecodeGenerationOutput.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::bytecode::ExecutionContext);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::ExecutionContext)

ska::bytecode::ExecutionContext::ExecutionContext(ExecutionOutput& container, std::size_t scriptIndex, GenerationOutput& instructions) :
	m_container(container),
	m_bytecode(instructions),
	m_current(container.script(scriptIndex, instructions)) {
	if (instructions.generated().size() <= scriptIndex) {
		throw std::runtime_error("you must compile a script to bytecode before trying to execute it.");
	}
}

ska::bytecode::ScriptExecutionOutput ska::bytecode::ExecutionContext::generateExportedVariables(std::size_t scriptIndex) const {
	auto result = std::make_shared<NodeValueArrayRaw>();
	const auto exportedSymbolsVariables = m_bytecode.generateExportedSymbols(scriptIndex);
	for (const auto& variable : exportedSymbolsVariables) {
		result->push_back(getCell(variable));
	}
	return result;
}

const ska::bytecode::ScriptGenerationOutput& ska::bytecode::ExecutionContext::generateIfNeeded(Generator& generator, std::size_t scriptIndex) {
	if (m_bytecode.generated().size() <= scriptIndex) {
		auto importedScriptContext = GenerationContext{ m_bytecode, scriptIndex };
		m_bytecode.setOut(scriptIndex, generator.generatePart(importedScriptContext));
	}
	return m_bytecode.generated()[scriptIndex];
}

ska::bytecode::ExecutionContext ska::bytecode::ExecutionContext::getContext(ScriptVariableRef value) {
	return ExecutionContext{ *this, value.script };	
}

void ska::bytecode::ExecutionContext::jumpAbsolute(ScriptVariableRef value) {
	auto context = getContext(value);
	m_container.callstack.push_back(TokenVariant{ m_current->snapshot() });
	m_current = context.m_current;
	m_current->jumpAbsolute(value.variable - 1);
}

ska::bytecode::ScriptVariableRef ska::bytecode::ExecutionContext::getReturn() {
	assert(!m_container.callstack.empty());
	auto scriptVariableRef = m_container.callstack.back().nodeval<ScriptVariableRef>();
	m_container.callstack.pop_back();
	return scriptVariableRef;
}

void ska::bytecode::ExecutionContext::jumpReturn() {
	auto whereToGo = getReturn();
	LOG_DEBUG << "Returning to instruction index " << whereToGo.variable << " in script " << whereToGo.script;
	auto context = getContext(whereToGo);
	m_current = context.m_current;
	m_current->jumpAbsolute(whereToGo.variable);
}
