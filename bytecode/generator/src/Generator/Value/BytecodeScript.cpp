#include <sstream>
#include "Config/LoggerConfigLang.h"
#include "NodeValue/AST.h"
#include "BytecodeScript.h"
#include "NodeValue/ScriptAST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::Script);
SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::UniqueSymbolGetterBase);

ska::bytecode::Script::Script(ska::ScriptAST& script) :
	m_script(script.handle()) {
}

ska::bytecode::Register ska::bytecode::Script::queryNextRegister() {
	return { m_register++, ValueType::REG };
}

ska::bytecode::Value ska::bytecode::Script::querySymbolOrValue(const ASTNode& node) {
	/*
	switch(node.type().value().symbolType().type()) {
	case ExpressionType::FUNCTION:
		return LabelGetter::query(node);
	default:
	*/
		return VariableGetter::query(node);
	/*}*/
}

ska::bytecode::Value ska::bytecode::Script::queryLabel(const ASTNode& node) {
	return LabelGetter::query(node);
}

ska::bytecode::Value ska::bytecode::UniqueSymbolGetterBase::query(const ASTNode& node) {
	if (node.symbol() == nullptr) {
		SLOG(ska::LogLevel::Debug) << "Querying symbol node with value " << node.name();
		return Value { node };
	}

	auto varCount = m_container.find(node.symbol());
	if (varCount == m_container.end()) {
		varCount = m_container.emplace(node.symbol(), m_count++).first;
	}
	auto ss = std::stringstream{};
	ss << m_symbol << varCount->second;

	SLOG(ska::LogLevel::Debug) << "Querying symbol node " << node.name() << " with value " << ss.str();

	return { varCount->second, m_symbol == 'V' ? ValueType::VAR : ValueType::LBL };
}
