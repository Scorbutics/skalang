#include "Config/LoggerConfigLang.h"
#include "NodeValue/AST.h"
#include "BytecodeGenerationContext.h"
#include "Generator/Value/BytecodeScriptGenerationService.h"
#include "Generator/Value/BytecodeGenerationOutput.h"
#include "NodeValue/ScriptAST.h"

ska::bytecode::GenerationContext::GenerationContext(GenerationOutput& output, std::size_t scriptIndex) :
	m_generated(output),
	m_script(nullptr),
	m_scriptIndex((scriptIndex == std::numeric_limits<std::size_t>::max()) ? output.size() - 1 : scriptIndex),
	m_pointer(&output.backService().program().rootNode()) {
}

static inline ska::bytecode::ScriptGenerationService* ScriptFromOutput(ska::bytecode::GenerationOutput& output, ska::bytecode::ScriptGenerationService script) {
	output.push(std::move(script));
	return nullptr;
}

std::size_t ska::bytecode::GenerationContext::totalScripts() const {
	return m_generated.size();
}

ska::bytecode::GenerationContext::GenerationContext(GenerationContext& old, ScriptGenerationService script) :
	m_generated(old.m_generated),
	m_script(ScriptFromOutput(m_generated, std::move(script))),
	m_scriptIndex(m_generated.size() - 1),
	m_pointer(&m_generated.script(m_scriptIndex).program().rootNode()) {
}

ska::bytecode::GenerationContext::GenerationContext(GenerationContext& old) :
	m_generated(old.m_generated),
	m_script(old.m_script),
	m_scriptIndex(old.m_scriptIndex),
	m_pointer(&m_generated.script(m_scriptIndex).program().rootNode()) {
}

ska::bytecode::GenerationContext::GenerationContext(GenerationContext& old, const ASTNode& node, std::size_t scopeLevelOffset) :
	m_generated(old.m_generated),
	m_script(old.m_script),
	m_scriptIndex(old.m_scriptIndex),
	m_pointer(&node),
	m_scopeLevel(old.m_scopeLevel + scopeLevelOffset) {
}

ska::bytecode::ScriptGenerationService& ska::bytecode::GenerationContext::script() {
	if (m_script == nullptr) {
		return m_generated.script(m_scriptIndex);
	}

	assert(m_script != nullptr);
	return *m_script;
}

const ska::bytecode::ScriptGenerationService& ska::bytecode::GenerationContext::script() const {
	if (m_script == nullptr) {
		return m_generated.script(m_scriptIndex);
	}

	assert(m_script != nullptr);
	return *m_script;
}

std::pair<std::size_t, ska::bytecode::ScriptGenerationService*> ska::bytecode::GenerationContext::script(const std::string& fullScriptName) {
	return m_generated.script(fullScriptName);
}

void ska::bytecode::GenerationContext::setSymbolInfo(const ASTNode& node, SymbolInfo info) { 
	m_generated.setSymbolInfo(node, std::move(info));
}
const ska::bytecode::SymbolInfo* ska::bytecode::GenerationContext::getSymbolInfo(const Symbol& symbol) const { return m_generated.getSymbolInfo(symbol); }
const ska::bytecode::SymbolInfo* ska::bytecode::GenerationContext::getSymbolInfo(const ASTNode& node) const { return m_generated.getSymbolInfo(node); }

ska::bytecode::Value ska::bytecode::GenerationContext::querySymbolOrValue(const ASTNode& node) { 
	if (node.symbol() == nullptr) {
		return script().querySymbolOrValue(node);
	}
	return scriptOfSymbol(*node.symbol()).querySymbolOrValue(node); 
}

ska::bytecode::Value ska::bytecode::GenerationContext::querySymbol(const Symbol& symbol) { 
	return scriptOfSymbol(symbol).querySymbol(symbol);
}

std::optional<ska::bytecode::Value> ska::bytecode::GenerationContext::getSymbol(const Symbol& symbol) const { 
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
	return m_generated.script(scriptIt->script);
}

const ska::bytecode::ScriptGenerationService& ska::bytecode::GenerationContext::scriptOfSymbol(const Symbol& symbol) const {
	const auto* scriptIt = m_generated.getSymbolInfo(symbol);
	if (scriptIt == nullptr) {
		return script();
	}
	return m_generated.script(scriptIt->script);
}
