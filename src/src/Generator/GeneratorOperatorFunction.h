#pragma once

#include "GeneratorOperator.h"
#include "Operation/Type/OperationTypeFunctionDeclaration.h"
#include "Operation/Type/OperationTypeFunctionCall.h"

namespace ska {
	namespace bytecode {
		SKALANG_GENERATOR_OPERATOR_DEFINE(Operator::FUNCTION_CALL);
		SKALANG_GENERATOR_OPERATOR_DEFINE(Operator::FUNCTION_DECLARATION);
	}
}
