#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "MemoryTable.h"
#include "InterpreterOperatorBridge.h"
#include "Service/Script.h"

SKA_LOGC_CONFIG(ska::LogLevel::Info, ska::InterpreterOperator<ska::Operator::BRIDGE>);

ska::NodeCell ska::InterpreterOperator<ska::Operator::BRIDGE>::interpret(OperateOn node) {
	auto bridgeCellData = node.parent.symbols()[node.GetObject().name()];

	if (bridgeCellData == nullptr) {
		throw std::runtime_error("unable to find the bridged script \"" + node.GetObject().name() + "\" in the current memory");
	}
	auto script = node.parent.subScript(bridgeCellData->getName());
	auto ec = ExecutionContext{ *script };
	return ska::NodeCell{ ec };
}
