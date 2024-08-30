#include <fstream>
#include "Config/LoggerConfigLang.h"
#include "Generator/BytecodeGenerator.h"
#include "Generator/BytecodeGenerationContext.h"
#include "BytecodeExecutionContext.h"

#include "Service/StatementParser.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::bytecode::ExecutionContext);
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

ska::bytecode::ExecutionContext ska::bytecode::ExecutionContext::getContext(const ScriptVariableRef& value) {
	return ExecutionContext{ *this, value.script };
}

void ska::bytecode::ExecutionContext::jumpAbsolute(NodeValue& value) {
	const auto& variable = value.nodeval<ScriptVariableRef>();
	auto context = getContext(variable);
	m_out.callstack.push_back(TokenVariant{ m_current->snapshot() });
	m_out.closureEnvironment.push_back(value.env());
	m_current = context.m_current;
	checkCurrentExecutionOrThrow();
	m_current->jumpAbsolute(variable.variable - 1);
}

const ska::NodeValue *ska::bytecode::ExecutionContext::useFromCurrentEnv(const ScriptVariableRef &dest) const {
	if (m_out.closureEnvironment.empty()) {
		return nullptr;
	}

	// TODO store env in script and access it with scriptFromOperand?
	auto* currentEnvironment = m_out.closureEnvironment.back();
	return currentEnvironment == nullptr ? nullptr : currentEnvironment->get_if(dest.variable);
}

ska::NodeValue ska::bytecode::ExecutionContext::getCell(const Operand &variable) const {
	const auto* value = variable.type() == OperandType::VAR || variable.type() == OperandType::REG ? useFromCurrentEnv(variable.as<ScriptVariableRef>()) : nullptr;
	return value == nullptr || value->empty() ? scriptFromOperand(variable).getCell(variable) : *value;
	//return scriptFromOperand(variable).getCell(variable);
}

ska::NodeValue ska::bytecode::ExecutionContext::getReturn() {
    assert(!m_out.callstack.empty());
	auto result = m_out.callstack.back();
	m_out.popEnv(result);
	m_out.callstack.pop_back();
	if (m_out.closureEnvironment.back() != nullptr) {
		LOG_DEBUG << "Poping env: " << *m_out.closureEnvironment.back();
	}
	m_out.closureEnvironment.pop_back();
	return result;
}

ska::NodeValue ska::bytecode::ExecutionContext::jumpReturn() {
	auto ret = getReturn();
	auto whereToGo = ret.nodeval<ScriptVariableRef>();
	LOG_DEBUG << "Returning to instruction index " << whereToGo.variable << " in script " << whereToGo.script;
	auto context = getContext(whereToGo);
	m_current = context.m_current;
	checkCurrentExecutionOrThrow();
	m_current->jumpAbsolute(whereToGo.variable);
	return ret;
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
		auto* result = m_out.script(scriptIndex, m_in);
		if(result == nullptr) {
			throw std::runtime_error("not a valid script at index " + std::to_string(scriptIndex));
		}
		return *result;
	}
	checkCurrentExecutionOrThrow();
	return *m_current;
}

const ska::NativeFunction& ska::bytecode::ExecutionContext::getBinding(const ScriptVariableRef& bindingRef) const {
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
