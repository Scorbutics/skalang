#pragma once

#include "InterpreterOperator.h"
#include "Operation/Interpreter/OperationIfElse.h"

namespace ska {
	SKALANG_INTERPRETER_OPERATOR_DEFINE(Operator::IF);
    SKALANG_INTERPRETER_OPERATOR_DEFINE(Operator::IF_ELSE);
}
