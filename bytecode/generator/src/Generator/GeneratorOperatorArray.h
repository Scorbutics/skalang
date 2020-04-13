#pragma once

#include "GeneratorOperator.h"
#include "Operation/OperationTypeArrayDeclaration.h"
#include "Operation/OperationTypeArrayUse.h"

namespace ska {
	namespace bytecode {
		SKALANG_GENERATOR_OPERATOR_DEFINE(Operator::ARRAY_TYPE_DECLARATION);
		SKALANG_GENERATOR_OPERATOR_DEFINE(Operator::ARRAY_USE);
	}
}
