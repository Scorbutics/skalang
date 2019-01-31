#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "MemoryTable.h"
#include "InterpreterOperatorImport.h"

SKA_LOGC_CONFIG(ska::LogLevel::Info, ska::InterpreterOperator<ska::Operator::IMPORT>);

ska::NodeCell ska::InterpreterOperator<ska::Operator::IMPORT>::interpret(OperateOn node) {
	auto scriptPath = node.GetValueNode().name();
	auto scriptMemory = m_memory["import " + scriptPath];
	if(scriptMemory.first != nullptr) {
		SLOG_STATIC(ska::LogLevel::Info, ska::InterpreterOperator<ska::Operator::IMPORT>) << "Script " << scriptPath << " found in cache, loading from memory";
		return scriptMemory.first->clone();
	}
	SLOG_STATIC(ska::LogLevel::Info, ska::InterpreterOperator<ska::Operator::IMPORT>) << "Script " << scriptPath << " loading from file";
	auto scriptValue = m_interpreter.interpret(node.GetScriptNode());
	return m_memory.emplace("import " + scriptPath, scriptValue.asRvalue()).first->clone();
}
