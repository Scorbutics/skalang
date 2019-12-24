#include "Config/LoggerConfigLang.h"
#include "NodeValue/AST.h"
#include "BytecodeGenerationContext.h"
#include "Generator/Value/BytecodeScriptGenerationHelper.h"
#include "Generator/Value/BytecodeGenerationOutput.h"
#include "NodeValue/ScriptAST.h"

ska::bytecode::GenerationContext::GenerationContext(GenerationOutput& output, ScriptGenerationHelper script) :
	m_generated(output),
	m_script(m_generated.emplaceNamed( ScriptGeneration{ std::move(script) } )),
	m_pointer(&m_script.rootASTNode()) {
}

ska::bytecode::GenerationContext::GenerationContext(GenerationOutput& output, const ScriptAST& scriptAst) :
	m_generated(output),
	m_script(m_generated.emplaceNamed(ScriptGeneration{ ScriptGenerationHelper{output, scriptAst} })),
	m_pointer(&m_script.rootASTNode()) {
}

ska::bytecode::GenerationContext::GenerationContext(GenerationContext& old, const ScriptAST& scriptAst) :
	m_generated(old.m_generated),
	m_script(m_generated.emplaceNamed(ScriptGeneration { ScriptGenerationHelper{m_generated, scriptAst} })),
	m_pointer(&m_script.rootASTNode()) {
}

std::size_t ska::bytecode::GenerationContext::totalScripts() const {
	return m_generated.size();
}

ska::bytecode::GenerationContext::GenerationContext(GenerationContext& old, ScriptGenerationHelper script) :
	m_generated(old.m_generated),
	m_script(m_generated.emplaceNamed(ScriptGeneration { std::move(script) })),
	m_pointer(&m_script.rootASTNode()) {
}

ska::bytecode::GenerationContext::GenerationContext(GenerationContext& old) :
	m_generated(old.m_generated),
	m_script(old.m_script),
	m_pointer(&m_script.rootASTNode()) {
}

ska::bytecode::GenerationContext::GenerationContext(GenerationContext& old, const ASTNode& node, std::size_t scopeLevelOffset) :
	m_generated(old.m_generated),
	m_script(old.m_script),
	m_pointer(&node),
	m_scopeLevel(old.m_scopeLevel + scopeLevelOffset) {
}

ska::bytecode::ScriptGenerationHelper& ska::bytecode::GenerationContext::helper() {
	return m_script.helper();
}

const ska::bytecode::ScriptGenerationHelper& ska::bytecode::GenerationContext::helper() const {
	return m_script.helper();
}

ska::bytecode::Register ska::bytecode::GenerationContext::queryNextRegister() {
	return m_script.helper().queryNextRegister();
}

std::pair<std::size_t, ska::bytecode::ScriptGeneration*> ska::bytecode::GenerationContext::script(const std::string& fullScriptName) {
	return std::make_pair(m_generated.id(fullScriptName), m_generated.atOrNull(fullScriptName));
}

void ska::bytecode::GenerationContext::setSymbolInfo(const ASTNode& node, SymbolInfo info) {
	m_generated.setSymbolInfo(node, std::move(info));
}
const ska::bytecode::SymbolInfo* ska::bytecode::GenerationContext::getSymbolInfo(const Symbol& symbol) const { return m_generated.getSymbolInfo(symbol); }
const ska::bytecode::SymbolInfo* ska::bytecode::GenerationContext::getSymbolInfo(const ASTNode& node) const { return m_generated.getSymbolInfo(node); }

ska::bytecode::Operand ska::bytecode::GenerationContext::querySymbolOrOperand(const ASTNode& node) {
	if (node.symbol() == nullptr) {
		return helper().querySymbolOrOperand(node);
	}
	return scriptOfSymbol(*node.symbol()).querySymbolOrOperand(node);
}

ska::bytecode::Operand ska::bytecode::GenerationContext::querySymbol(const Symbol& symbol) {
	return scriptOfSymbol(symbol).querySymbol(symbol);
}

std::optional<ska::bytecode::Operand> ska::bytecode::GenerationContext::getSymbol(const Symbol& symbol) const {
	return scriptOfSymbol(symbol).getSymbol(symbol);
}

ska::bytecode::ScriptGenerationHelper& ska::bytecode::GenerationContext::scriptOfSymbol(const Symbol& symbol) {
	const auto* scriptIt = m_generated.getSymbolInfo(symbol);
	if (scriptIt == nullptr) {
		auto basicInfos = SymbolInfo{};
		basicInfos.script = m_script.id();
		m_generated.setSymbolInfo(symbol, std::move(basicInfos));
		return helper();
	}
	return m_generated.at(scriptIt->script).helper();
}

const ska::bytecode::ScriptGenerationHelper& ska::bytecode::GenerationContext::scriptOfSymbol(const Symbol& symbol) const {
	const auto* scriptIt = m_generated.getSymbolInfo(symbol);
	if (scriptIt == nullptr) {
		return helper();
	}
	return m_generated.at(scriptIt->script).helper();
}

ska::ScriptASTPtr ska::bytecode::GenerationContext::useImport(const std::string& scriptImported) {
	return helper().useImport(scriptImported);
}
