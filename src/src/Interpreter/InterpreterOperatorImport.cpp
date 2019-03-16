#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "MemoryTable.h"
#include "InterpreterOperatorImport.h"
#include "Service/Script.h"

SKA_LOGC_CONFIG(ska::LogLevel::Info, ska::InterpreterOperator<ska::Operator::IMPORT>);

ska::NodeCell ska::InterpreterOperator<ska::Operator::IMPORT>::interpret(OperateOn node) {
	auto scriptPath = node.GetValueNode().name();
	auto scriptExecutionContext = ExecutionContext{ node.GetScript() };
	//if (!node.parent.existsInCache(scriptPath + ".miniska")) {
		m_interpreter.interpret(scriptExecutionContext);
		SLOG_STATIC(ska::LogLevel::Info, ska::InterpreterOperator<ska::Operator::IMPORT>) << "Script " << " loading from file";
	/*} else {
		node.parent.rewind();
		SLOG_STATIC(ska::LogLevel::Info, ska::InterpreterOperator<ska::Operator::IMPORT>) << "Script " << " found in cache, loading from memory";
	}*/
	return scriptExecutionContext;
}
