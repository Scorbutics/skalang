#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "MemoryTable.h"
#include "InterpreterOperatorBridge.h"
#include "Service/Script.h"

SKA_LOGC_CONFIG(ska::LogLevel::Info, ska::InterpreterOperator<ska::Operator::BRIDGE>);

ska::NodeCell ska::InterpreterOperator<ska::Operator::BRIDGE>::interpret(OperateOn node) {
	return node.parent.memory()[node.GetObject().name()];
}
