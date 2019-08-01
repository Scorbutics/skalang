#include <sstream>
#include "Config/LoggerConfigLang.h"
#include "NodeValue/AST.h"
#include "BytecodeScript.h"
#include "Interpreter/Value/Script.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::Script);

ska::bytecode::Script::Script(ska::Script& script) :
	m_script(script.handle()) {
}

ska::bytecode::Register ska::bytecode::Script::queryNextRegister(Type type) {
	auto ss = std::stringstream{};
	ss << "R" << m_register++;
	return { ss.str(), std::move(type) };
}

ska::bytecode::Value ska::bytecode::Script::queryVariableOrValue(const ASTNode& node, const Type* type) {
	if (node.symbol() == nullptr) {
		SLOG(ska::LogLevel::Debug) << "Querying variable node with value " << node.name();
		return { node.name(), type == nullptr ? node.type().value() : *type };
	}

	auto varCount = m_variables.find(node.symbol());
	if (varCount == m_variables.end()) {
		varCount = m_variables.emplace(node.symbol(), m_variableCount++).first;
	}
	auto ss = std::stringstream{};
	ss << "V" << varCount->second;

	SLOG(ska::LogLevel::Debug) << "Querying variable node " << node.name() << " with value " << ss.str();

	return { ss.str(), node.type().value() };
}
