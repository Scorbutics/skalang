#include "Config/LoggerConfigLang.h"
#include "NodeValue/AST.h"
#include "BytecodeGenerationContext.h"
#include "Generator/Value/BytecodeScriptGenerationService.h"
#include "Generator/Value/BytecodeGenerationOutput.h"
#include "NodeValue/ScriptAST.h"

ska::bytecode::GenerationContext::GenerationContext(GenerationOutput& output, std::size_t scriptIndex) :
	m_generated(output),
	m_scriptIndex((scriptIndex == std::numeric_limits<std::size_t>::max()) ? output.size() - 1 : scriptIndex),
	m_pointer(&output.genCache.at(m_scriptIndex).rootASTNode()) {
}

static inline std::size_t ScriptFromOutput(ska::bytecode::GenerationOutput& output, ska::bytecode::ScriptGenerationService script) {
	auto scriptName = script.name();
	output.genCache.emplace(scriptName, std::move(script));
	return output.genCache.id(scriptName);
}

std::size_t ska::bytecode::GenerationContext::totalScripts() const {
	return m_generated.size();
}

ska::bytecode::GenerationContext::GenerationContext(GenerationContext& old, ScriptGenerationService script) :
	m_generated(old.m_generated),
	m_scriptIndex(ScriptFromOutput(m_generated, std::move(script))),
	m_pointer(&m_generated.genCache.at(m_scriptIndex).program().rootNode()) {
}

ska::bytecode::GenerationContext::GenerationContext(GenerationContext& old) :
	m_generated(old.m_generated),
	m_scriptIndex(old.m_scriptIndex),
	m_pointer(&m_generated.genCache.at(m_scriptIndex).program().rootNode()) {
}

ska::bytecode::GenerationContext::GenerationContext(GenerationContext& old, const ASTNode& node, std::size_t scopeLevelOffset) :
	m_generated(old.m_generated),
	m_scriptIndex(old.m_scriptIndex),
	m_pointer(&node),
	m_scopeLevel(old.m_scopeLevel + scopeLevelOffset) {
}

ska::bytecode::ScriptGenerationService& ska::bytecode::GenerationContext::script() {
	return m_generated.genCache.at(m_scriptIndex);
}

const ska::bytecode::ScriptGenerationService& ska::bytecode::GenerationContext::script() const {
	return m_generated.genCache.at(m_scriptIndex);
}

std::pair<std::size_t, ska::bytecode::ScriptGenerationService*> ska::bytecode::GenerationContext::script(const std::string& fullScriptName) {
	return std::make_pair(m_generated.genCache.id(fullScriptName), &m_generated.genCache.at(fullScriptName));
}

void ska::bytecode::GenerationContext::setSymbolInfo(const ASTNode& node, SymbolInfo info) {
	m_generated.setSymbolInfo(node, std::move(info));
}
const ska::bytecode::SymbolInfo* ska::bytecode::GenerationContext::getSymbolInfo(const Symbol& symbol) const { return m_generated.getSymbolInfo(symbol); }
const ska::bytecode::SymbolInfo* ska::bytecode::GenerationContext::getSymbolInfo(const ASTNode& node) const { return m_generated.getSymbolInfo(node); }

ska::bytecode::Operand ska::bytecode::GenerationContext::querySymbolOrOperand(const ASTNode& node) {
	if (node.symbol() == nullptr) {
		return script().querySymbolOrOperand(node);
	}
	return scriptOfSymbol(*node.symbol()).querySymbolOrOperand(node);
}

ska::bytecode::Operand ska::bytecode::GenerationContext::querySymbol(const Symbol& symbol) {
	return scriptOfSymbol(symbol).querySymbol(symbol);
}

std::optional<ska::bytecode::Operand> ska::bytecode::GenerationContext::getSymbol(const Symbol& symbol) const {
	return scriptOfSymbol(symbol).getSymbol(symbol);
}

ska::bytecode::ScriptGenerationService& ska::bytecode::GenerationContext::scriptOfSymbol(const Symbol& symbol) {
	const auto* scriptIt = m_generated.getSymbolInfo(symbol);
	if (scriptIt == nullptr) {
		auto basicInfos = SymbolInfo{};
		basicInfos.script = m_scriptIndex;
		m_generated.setSymbolInfo(symbol, std::move(basicInfos));
		return script();
	}
	return m_generated.genCache.at(scriptIt->script);
}

const ska::bytecode::ScriptGenerationService& ska::bytecode::GenerationContext::scriptOfSymbol(const Symbol& symbol) const {
	const auto* scriptIt = m_generated.getSymbolInfo(symbol);
	if (scriptIt == nullptr) {
		return script();
	}
	return m_generated.genCache.at(scriptIt->script);
}
