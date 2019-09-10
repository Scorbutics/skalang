#include <sstream>
#include "Config/LoggerConfigLang.h"
#include "NodeValue/AST.h"
#include "BytecodeScript.h"
#include "NodeValue/ScriptAST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::ScriptGenerationService);
SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::UniqueSymbolGetterBase);

ska::bytecode::ScriptGenerationService::ScriptGenerationService(ska::ScriptAST& script) :
	m_script(script.handle()) {
}

ska::bytecode::Register ska::bytecode::ScriptGenerationService::queryNextRegister() {
	return { VariableRef { m_register++ }, ValueType::REG };
}

ska::bytecode::Value ska::bytecode::ScriptGenerationService::querySymbolOrValue(const ASTNode& node) {
	/*
	switch(node.type().value().symbolType().type()) {
	case ExpressionType::FUNCTION:
		return LabelGetter::query(node);
	default:
	*/
		return VariableGetter::query(node).first;
	/*}*/
}

ska::bytecode::Value ska::bytecode::ScriptGenerationService::querySymbol(const Symbol& symbol) {
	return VariableGetter::query(symbol).first;
}
void ska::bytecode::ScriptGenerationService::setSymbolInfo(const ASTNode& node, SymbolInfo info) {
	if (node.symbol() == nullptr) {
		throw std::runtime_error("Cannot set symbol information for a node without symbol : " + node.name());
	}

	m_symbolInfo.emplace(node.symbol(), std::move(info));
}

const ska::bytecode::SymbolInfo* ska::bytecode::ScriptGenerationService::getSymbolInfo(const Symbol& symbol) const {
	if(m_symbolInfo.find(&symbol) == m_symbolInfo.end()) {
		return nullptr;
	}
	return &m_symbolInfo.at(&symbol);
}

const ska::bytecode::SymbolInfo* ska::bytecode::ScriptGenerationService::getSymbolInfo(const ASTNode& node) const {
	if(node.symbol() == nullptr) {
		return nullptr;
	}
	return getSymbolInfo(*node.symbol());
}

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
