#include "Config/LoggerConfigLang.h"
#include "Generator/BytecodeGenerator.h"
#include "Generator/BytecodeGenerationContext.h"
#include "BytecodeExecutionContext.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::ExecutionContext);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::ExecutionContext)

ska::bytecode::ExecutionContext::ExecutionContext(Executor& container, std::size_t scriptIndex, GenerationOutput& instructions) :
	m_out(container),
	m_in(instructions),
	m_current(container.script(scriptIndex, instructions)) {
	if (!instructions.isGenerated(scriptIndex)) {
		throw std::runtime_error("you must compile a script to bytecode before trying to execute it.");
	}
}

ska::bytecode::ScriptExecutionOutput ska::bytecode::ExecutionContext::generateExportedVariables(std::size_t scriptIndex) {
	auto* scriptExecution = m_out.script(scriptIndex);
	assert(scriptExecution != nullptr);
	const auto& symbols = scriptExecution->exports();
	if(symbols == nullptr) {
		auto result = std::make_shared<NodeValueArrayRaw>();
		const auto& exportedSymbolsVariables = m_in.getExportedSymbols(scriptIndex);
		for (const auto& variable : exportedSymbolsVariables) {
			result->push_back(getCell(variable));
		}
		scriptExecution->setExportsSection(result);
		return result;
	}
	SLOG(ska::LogLevel::Info) << "No generation of exported symbols for script \"" << scriptIndex << "\" required";
	return symbols;
}

//TODO : why can we generate from execution context ?! weird
const ska::bytecode::ScriptGeneration& ska::bytecode::ExecutionContext::generateIfNeeded(Generator& generator, std::size_t scriptIndex) {
	if (!m_in.isGenerated(scriptIndex)) {
		m_in.at(scriptIndex).generate(m_in, generator);
	}
	return m_in[scriptIndex];
}

ska::bytecode::ExecutionContext ska::bytecode::ExecutionContext::getContext(ScriptVariableRef value) {
	return ExecutionContext{ *this, value.script };
}

void ska::bytecode::ExecutionContext::jumpAbsolute(ScriptVariableRef value) {
	auto context = getContext(value);
	m_out.callstack.push_back(TokenVariant{ m_current->snapshot() });
	m_current = context.m_current;
	m_current->jumpAbsolute(value.variable - 1);
}

ska::ScriptVariableRef ska::bytecode::ExecutionContext::getReturn() {
	assert(!m_out.callstack.empty());
	auto scriptVariableRef = m_out.callstack.back().nodeval<ScriptVariableRef>();
	m_out.callstack.pop_back();
	return scriptVariableRef;
}

void ska::bytecode::ExecutionContext::jumpReturn() {
	auto whereToGo = getReturn();
	LOG_DEBUG << "Returning to instruction index " << whereToGo.variable << " in script " << whereToGo.script;
	auto context = getContext(whereToGo);
	m_current = context.m_current;
	m_current->jumpAbsolute(whereToGo.variable);
}

ska::bytecode::ScriptExecution& ska::bytecode::ExecutionContext::scriptFromOperand(const Operand& v) {
	if (std::holds_alternative<ScriptVariableRef>(v.content())) {
		const auto scriptIndex = v.as<ScriptVariableRef>().script;
		auto* result = m_out.script(scriptIndex, m_in);
		if(result == nullptr) {
			throw std::runtime_error("not a valid script at index " + std::to_string(scriptIndex));
		}
		return *result;
	}
	return *m_current;
}

const ska::bytecode::ScriptExecution& ska::bytecode::ExecutionContext::scriptFromOperand(const Operand& v) const {
	if (std::holds_alternative<ScriptVariableRef>(v.content())) {
		const auto scriptIndex = v.as<ScriptVariableRef>().script;
		auto* result = m_out.script(scriptIndex);
		if(result == nullptr) {
			throw std::runtime_error("not a valid script at index " + std::to_string(scriptIndex));
		}
		return *result;
	}
	return *m_current;
}
