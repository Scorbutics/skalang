#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "InterpreterOperatorFunctionCall.h"

ska::NodeCell ska::InterpreterOperator<ska::Operator::FUNCTION_CALL>::interpret(const SymbolTable& symbols, MemoryTable& memory, ASTNode& node) {
	return "";
}
