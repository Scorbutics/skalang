#include <sstream>
#include "Config/LoggerConfigLang.h"
#include "BytecodeScriptGenerationService.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::bytecode::ScriptGenerationService);

ska::bytecode::ScriptGenerationService::ScriptGenerationService(std::size_t scriptIndex, ska::ScriptAST& script) :
	m_script(script.handle()),
	m_index(scriptIndex) {
}

ska::bytecode::Register ska::bytecode::ScriptGenerationService::queryNextRegister() {
	return { ScriptVariableRef { m_register++ }, ValueType::REG };
}

ska::bytecode::Value ska::bytecode::ScriptGenerationService::querySymbolOrValue(const ASTNode& node) {
	return VariableGetter::query(m_index, node).first;
}

ska::bytecode::Value ska::bytecode::ScriptGenerationService::querySymbol(const Symbol& symbol) {
	return VariableGetter::query(m_index, symbol).first;
}

std::optional<ska::bytecode::Value> ska::bytecode::ScriptGenerationService::getSymbol(const Symbol& symbol) const {
	return VariableGetter::get(m_index, symbol);
}

std::vector<ska::bytecode::Value> ska::bytecode::ScriptGenerationService::generateExportedSymbols(std::priority_queue<SymbolWithInfo> symbolsInfo) const {
	auto result = std::vector<Value>{};
	result.reserve(symbolsInfo.size());

	while (!symbolsInfo.empty()) {
		const auto& symbolWithInfo = symbolsInfo.top();
		auto value = VariableGetter::get(m_index, *symbolWithInfo.symbol);
		if (value.has_value()) {
			result.push_back(std::move(value.value()));
		}
		symbolsInfo.pop();
	}

	return result;
}
