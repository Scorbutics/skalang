#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "MemoryTable.h"
#include "InterpreterOperatorImport.h"
#include "Interpreter/Value/Script.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::InterpreterOperator<ska::Operator::IMPORT>);

ska::NodeCell ska::InterpreterOperator<ska::Operator::IMPORT>::interpret(OperateOn node) {
	auto boundScript = node.GetScript();
	assert(boundScript != nullptr && "invalid bound script (null)");
	assert(boundScript->handle() != nullptr && "invalid bound script (handle is null)");
	auto scriptExecutionContext = ExecutionContext{ *boundScript->handle() };
	m_interpreter.interpret(scriptExecutionContext);
	SLOG_STATIC(ska::LogLevel::Info, ska::InterpreterOperator<ska::Operator::IMPORT>) << "Script loading";
	return NodeRValue{ ScriptVariableRef{ std::numeric_limits<std::size_t>::max(), boundScript->id() }, nullptr };
}