#include <sstream>
#include "Config/LoggerConfigLang.h"
#include "NodeValue/AST.h"
#include "BytecodeScript.h"
#include "Interpreter/Value/Script.h"

ska::bytecode::Script::Script(ska::Script& script) :
	m_script(script.handle()) {
}

ska::bytecode::Register ska::bytecode::Script::queryNextRegister(Type type) {
	auto ss = std::stringstream{};
	ss << "R" << m_register++;
	return { ss.str(), std::move(type) };
}

ska::bytecode::Value ska::bytecode::Script::queryVariableOrValue(const ASTNode& node) {
	if (node.symbol() == nullptr) {
		return { node.name(), node.type().value() };
	}
		
	auto varCount = m_variables.find(node.symbol());
	if (varCount == m_variables.end()) {
		varCount = m_variables.emplace(node.symbol(), m_variableCount++).first;
	}
	auto ss = std::stringstream{};
	ss << "V" << varCount->second;
	return { ss.str(), node.type().value() };
}
