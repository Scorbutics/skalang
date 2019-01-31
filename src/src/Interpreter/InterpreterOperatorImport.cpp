#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "MemoryTable.h"
#include "InterpreterOperatorImport.h"

SKA_LOGC_CONFIG(ska::LogLevel::Info, ska::InterpreterOperator<ska::Operator::IMPORT>);

ska::NodeCell ska::InterpreterOperator<ska::Operator::IMPORT>::interpret(OperateOn node) {
	auto scriptPath = node.GetValueNode().name();
	const auto* scriptMemory = m_memory["import " + scriptPath].first;
	if(scriptMemory != nullptr) {
		SLOG_STATIC(ska::LogLevel::Info, ska::InterpreterOperator<ska::Operator::IMPORT>) << "Script " << scriptPath << " found in cache, loading from memory";
		return NodeCell {scriptMemory};
	}
	SLOG_STATIC(ska::LogLevel::Info, ska::InterpreterOperator<ska::Operator::IMPORT>) << "Script " << scriptPath << " loading from file";
	auto scriptValue = m_interpreter.interpret(node.GetScriptNode()).asRvalue();
	m_memory.emplace("import " + scriptPath, scriptValue.clone());
	return scriptValue;
}
