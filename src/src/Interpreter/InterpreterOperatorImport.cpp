#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "MemoryTable.h"
#include "InterpreterOperatorImport.h"
#include "Service/Script.h"

SKA_LOGC_CONFIG(ska::LogLevel::Info, ska::InterpreterOperator<ska::Operator::IMPORT>);

ska::NodeCell ska::InterpreterOperator<ska::Operator::IMPORT>::interpret(OperateOn node) {
	auto scriptPath = node.GetValueNode().name();
	auto scriptMemory = node.parent.memory()["import " + scriptPath];
	if(scriptMemory.first != nullptr) {
		SLOG_STATIC(ska::LogLevel::Info, ska::InterpreterOperator<ska::Operator::IMPORT>) << "Script " << scriptPath << " found in cache, loading from memory";
		return scriptMemory.first->clone();
	}
	SLOG_STATIC(ska::LogLevel::Info, ska::InterpreterOperator<ska::Operator::IMPORT>) << "Script " << scriptPath << " loading from file";
	//auto scriptValue = m_interpreter.interpret(node.GetScriptNode());
	auto scriptValue = node.GetScriptNode();
	return node.parent.memory().emplace("import " + scriptPath, scriptValue).first->clone();
}
