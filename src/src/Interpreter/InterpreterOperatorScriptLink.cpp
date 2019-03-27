#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "MemoryTable.h"
#include "InterpreterOperatorScriptLink.h"
#include "Service/Script.h"

SKA_LOGC_CONFIG(ska::LogLevel::Info, ska::InterpreterOperator<ska::Operator::SCRIPT_LINK>);

ska::NodeCell ska::InterpreterOperator<ska::Operator::SCRIPT_LINK>::interpret(OperateOn node) {
	auto scriptLinkCellData = node.parent.symbols()[node.GetObject().name()];

	if (scriptLinkCellData == nullptr) {
		throw std::runtime_error("unable to find the linked script \"" + node.GetObject().name() + "\" registered in the current symbol table");
	}
	auto script = node.parent.subScript(scriptLinkCellData->getName());
	auto ec = ExecutionContext{ *script };
	return ska::NodeCell{ ec };
}
