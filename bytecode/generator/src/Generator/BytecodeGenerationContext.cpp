#include "Config/LoggerConfigLang.h"
#include "NodeValue/AST.h"
#include "BytecodeGenerationContext.h"
#include "Generator/Value/BytecodeScript.h"
#include "Generator/Value/BytecodeGenerationOutput.h"
#include "NodeValue/ScriptAST.h"

ska::bytecode::GenerationContext::GenerationContext(GenerationOutput& output) :
	m_generated(output),
	m_script(&output.backService()),
	m_scriptIndex(output.size() - 1),
	m_pointer(&output.backService().program().rootNode()) {
}

static inline auto* ScriptFromOutput(ska::bytecode::GenerationOutput& output, ska::bytecode::ScriptGenerationService script, std::size_t& index) {
	index = output.push(std::move(script));
	return &output.backService();
}

ska::bytecode::GenerationContext::GenerationContext(GenerationContext& old, ScriptGenerationService script) :
	m_generated(old.m_generated),
	m_script(ScriptFromOutput(m_generated, std::move(script), m_scriptIndex)),
	m_pointer(&m_script->program().rootNode()) {
}

ska::bytecode::GenerationContext::GenerationContext(GenerationContext& old) :
	m_generated(old.m_generated),
	m_script(old.m_script),
	m_scriptIndex(old.m_scriptIndex),
	m_pointer(&old.m_script->program().rootNode()) {
}

ska::bytecode::GenerationContext::GenerationContext(GenerationContext& old, const ASTNode& node, std::size_t scopeLevelOffset) :
	m_generated(old.m_generated),
	m_script(old.m_script),
	m_scriptIndex(old.m_scriptIndex),
	m_pointer(&node),
	m_scopeLevel(old.m_scopeLevel + scopeLevelOffset) {
}

std::pair<std::size_t, ska::bytecode::ScriptGenerationService*> ska::bytecode::GenerationContext::script(const std::string& fullScriptName) {
	return m_generated.script(fullScriptName);
}

void ska::bytecode::GenerationContext::setSymbolInfo(const ASTNode& node, SymbolInfo info) { m_generated.setSymbolInfo(node, std::move(info)); }
const ska::bytecode::SymbolInfo* ska::bytecode::GenerationContext::getSymbolInfo(const Symbol& symbol) const { return m_generated.getSymbolInfo(symbol); }
const ska::bytecode::SymbolInfo* ska::bytecode::GenerationContext::getSymbolInfo(const ASTNode& node) const { return m_generated.getSymbolInfo(node); }
