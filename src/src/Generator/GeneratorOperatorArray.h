#pragma once

#include "GeneratorOperator.h"
#include "Operation/Type/OperationTypeArrayDeclaration.h"
#include "Operation/Type/OperationTypeArrayUse.h"

namespace ska {
	namespace bytecode {
		SKALANG_GENERATOR_OPERATOR_DEFINE(Operator::ARRAY_DECLARATION);
		SKALANG_GENERATOR_OPERATOR_DEFINE(Operator::ARRAY_USE);
	}
}
