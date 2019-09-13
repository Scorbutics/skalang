#include "Config/LoggerConfigLang.h"
#include "UniqueSymbolGetter.h"
#include "NodeValue/AST.h"
#include "NodeValue/ScriptAST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::bytecode::UniqueSymbolGetterBase);

std::pair<ska::bytecode::Value, bool> ska::bytecode::UniqueSymbolGetterBase::query(const ASTNode& node) {
	if (node.symbol() == nullptr) {
		SLOG(ska::LogLevel::Debug) << "Querying symbol node with value " << node.name();
		return std::make_pair(Value { node }, false);
	}

	return query(*node.symbol());
}

std::pair<ska::bytecode::Value, bool> ska::bytecode::UniqueSymbolGetterBase::query(const Symbol& symbol) {
	bool isNew = false;
	auto varCount = m_container.find(&symbol);
	if (varCount == m_container.end()) {
		varCount = m_container.emplace(&symbol, m_count++).first;
		isNew = true;
	}
	auto ss = std::stringstream{};
	ss << m_symbol << varCount->second;

	SLOG(ska::LogLevel::Debug) << "Querying symbol node " << symbol.getName() << " with value " << ss.str();

	return std::make_pair(Value { VariableRef{ varCount->second }, ValueType::VAR}, isNew);
}

std::optional<ska::bytecode::Value> ska::bytecode::UniqueSymbolGetterBase::query(const Symbol& symbol) const {
	auto varCount = m_container.find(&symbol);
	if (varCount == m_container.end()) {
		return {};
	}
	return Value{ VariableRef{ varCount->second }, ValueType::VAR };
}
