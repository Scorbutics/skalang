#include "Generator/BytecodeGenerator.h"
#include "Generator/BytecodeGenerationContext.h"
#include "BytecodeExecutionContext.h"
#include "Generator/Value/BytecodeGenerationOutput.h"

ska::bytecode::ExecutionContext::ExecutionContext(ExecutionOutput& container, std::size_t scriptIndex, GenerationOutput& instructions) :
	m_container(container),
	m_bytecode(instructions) {
	if (instructions.generated().size() <= scriptIndex) {
		throw std::runtime_error("you must compile a script to bytecode before trying to execute it.");
	}
	m_current = m_container.script(scriptIndex, instructions);
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
