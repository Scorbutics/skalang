#include <sstream>
#include "Config/LoggerConfigLang.h"
#include "NodeValue/AST.h"
#include "BytecodeScript.h"
#include "Interpreter/Value/Script.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::Script);
SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::UniqueSymbolGetterBase);

ska::bytecode::Script::Script(ska::Script& script) :
	m_script(script.handle()) {
}

ska::bytecode::Register ska::bytecode::Script::queryNextRegister() {
	auto ss = std::stringstream{};
	ss << "R" << m_register++;
	return { ss.str() };
}

ska::bytecode::Value ska::bytecode::Script::queryVariableOrValue(const ASTNode& node) {
	return VariableGetter::query(node);
}

ska::bytecode::Value ska::bytecode::Script::queryLabel(const ASTNode& node) {
	return LabelGetter::query(node);
}

ska::bytecode::Value ska::bytecode::UniqueSymbolGetterBase::query(const ASTNode& node) {
	if (node.symbol() == nullptr) {
		SLOG(ska::LogLevel::Debug) << "Querying symbol node with value " << node.name();
		return { node.name() };
	}

	auto varCount = m_container.find(node.symbol());
	if (varCount == m_container.end()) {
		varCount = m_container.emplace(node.symbol(), m_count++).first;
	}
	auto ss = std::stringstream{};
	ss << m_symbol << varCount->second;

	SLOG(ska::LogLevel::Debug) << "Querying symbol node " << node.name() << " with value " << ss.str();

	return { ss.str() };
}
