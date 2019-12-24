#include <sstream>
#include "Config/LoggerConfigLang.h"
#include "BytecodeScriptGenerationHelper.h"
#include "NodeValue/ScriptAST.h"
#include "BytecodeScriptCache.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::bytecode::ScriptGenerationHelper);

ska::bytecode::ScriptGenerationHelper::ScriptGenerationHelper(ScriptCache& cache, const ScriptAST& script) {
	const auto& name = script.name();
  /*if (cache.find(name) != cache.end()) {
		throw std::runtime_error("the script \"" + name + "\" already exists");
	}
*/
	m_script = script.handle();
	m_index = cache.id(name);
}

ska::ScriptAST ska::bytecode::ScriptGenerationHelper::program() const {
	return ska::ScriptAST{ *m_script };
}

const ska::ASTNode& ska::bytecode::ScriptGenerationHelper::rootASTNode() const {
	return m_script->rootNode();
}

const std::string& ska::bytecode::ScriptGenerationHelper::name() const {
	return m_script->name();
}

ska::bytecode::Register ska::bytecode::ScriptGenerationHelper::queryNextRegister() {
	return { ScriptVariableRef { m_register++ }, OperandType::REG };
}

ska::bytecode::Operand ska::bytecode::ScriptGenerationHelper::querySymbolOrOperand(const ASTNode& node) {
	return VariableGetter::query(m_index, node).first;
}

ska::bytecode::Operand ska::bytecode::ScriptGenerationHelper::querySymbol(const Symbol& symbol) {
	return VariableGetter::query(m_index, symbol).first;
}

std::optional<ska::bytecode::Operand> ska::bytecode::ScriptGenerationHelper::getSymbol(const Symbol& symbol) const {
	return VariableGetter::get(m_index, symbol);
}

std::vector<ska::bytecode::Operand> ska::bytecode::ScriptGenerationHelper::generateExportedSymbols(std::priority_queue<SymbolWithInfo> symbolsInfo) const {
	auto result = std::vector<Operand>{};
	result.reserve(symbolsInfo.size());

	while (!symbolsInfo.empty()) {
		const auto& symbolWithInfo = symbolsInfo.top();
		auto operand = VariableGetter::get(m_index, *symbolWithInfo.symbol);
		if (operand.has_value()) {
			result.push_back(std::move(operand.value()));
		}
		symbolsInfo.pop();
	}

	return result;
}

ska::ScriptASTPtr ska::bytecode::ScriptGenerationHelper::useImport(const std::string& scriptImported) {
	return program().useImport(scriptImported);
}
