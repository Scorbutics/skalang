#pragma once

#include "GeneratorOperator.h"
#include "Operation/OperationTypeFunctionDeclaration.h"
#include "Operation/OperationTypeFunctionCall.h"
#include "Operation/OperationTypeFunctionMemberCall.h"

namespace ska {
	namespace bytecode {
		SKALANG_GENERATOR_OPERATOR_DEFINE(Operator::FUNCTION_CALL);
		SKALANG_GENERATOR_OPERATOR_DEFINE(Operator::FUNCTION_MEMBER_CALL);
		SKALANG_GENERATOR_OPERATOR_DEFINE(Operator::FUNCTION_DECLARATION);
		SKALANG_GENERATOR_OPERATOR_DEFINE(Operator::FUNCTION_PROTOTYPE_DECLARATION);
	}
}
