#include "InterpreterCommandScript.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(SCRIPT)(ExecutionContext& context, const Value& left, const Value& right) {
	const auto& cell = context.currentInstruction().left();
	const auto importedScriptIndex = cell.as<ScriptVariableRef>().variable;
	context.generateIfNeeded(m_generator, importedScriptIndex);
	auto importedScriptExecutionContext = ExecutionContext {context, importedScriptIndex};
	m_interpreter.interpret(importedScriptExecutionContext);
	return importedScriptExecutionContext.generateExportedVariables(importedScriptIndex);
}
