#include <fstream>
#include "Config/LoggerConfigLang.h"
#include "Generator/BytecodeGenerator.h"
#include "Generator/BytecodeGenerationContext.h"
#include "BytecodeExecutionContext.h"

#include "Service/StatementParser.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::ExecutionContext);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::ExecutionContext)

ska::bytecode::ExecutionContext::ExecutionContext(Executor& container, std::size_t scriptIndex, GenerationOutput& instructions) :
	m_out(container),
	m_in(instructions),
	m_current(container.script(scriptIndex, instructions)) {
}

ska::bytecode::ScriptExecutionOutput ska::bytecode::ExecutionContext::generateExportedVariables(std::size_t scriptIndex) {
	auto* scriptExecution = m_out.script(scriptIndex);
	assert(scriptExecution != nullptr);
	const auto& symbols = scriptExecution->exports();
	if(symbols == nullptr) {
		auto result = std::make_shared<NodeValueArrayRaw>();
		const auto& exportedSymbolsVariables = m_in.getExportedSymbols(scriptIndex);
		for (const auto& variable : exportedSymbolsVariables) {
			result->push_back(getCell(variable.value().value));
		}
		scriptExecution->setExportsSection(result);
		return result;
	}
	SLOG(ska::LogLevel::Info) << "No generation of exported symbols for script \"" << scriptIndex << "\" required";
	return symbols;
}

bool ska::bytecode::ExecutionContext::isGenerated(std::size_t scriptIndex) const {
	return m_in.isGenerated(scriptIndex);
}

ska::bytecode::ExecutionContext ska::bytecode::ExecutionContext::getContext(ScriptVariableRef value) {
	return ExecutionContext{ *this, value.script };
}

void ska::bytecode::ExecutionContext::jumpAbsolute(ScriptVariableRef value) {
	auto context = getContext(value);
	m_out.callstack.push_back(TokenVariant{ m_current->snapshot() });
	m_current = context.m_current;
	checkCurrentExecutionOrThrow();
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
	checkCurrentExecutionOrThrow();
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
	checkCurrentExecutionOrThrow();
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
	checkCurrentExecutionOrThrow();
	return *m_current;
}

const ska::NativeFunction& ska::bytecode::ExecutionContext::getBinding(ScriptVariableRef bindingRef) const {
	return m_in.getBinding(bindingRef);
}

void ska::bytecode::ExecutionContext::generate(StatementParser& parser, Generator& generator) {
	if (!m_in.exist(currentScriptId())) {
		auto* name = m_in.findKey(currentScriptId());
		if (name == nullptr) {
			throw std::runtime_error("bad script id (key not found) : " + std::to_string(currentScriptId()));
		}
		auto file = std::ifstream { *name };
		auto scriptAst = parser.subParse(m_in.astCache, *name, file);
		generator.generate(m_in, ScriptGenerationHelper{ m_in, *scriptAst });
	} else {
		generator.generate(m_in, currentScriptId());
	}
	
}
