#pragma once

#include "GeneratorOperator.h"
#include "Operation/Type/OperationTypeVariableAffectation.h"
#include "Operation/Type/OperationTypeVariableDeclaration.h"

namespace ska {
	SKALANG_GENERATOR_OPERATOR_DEFINE(Operator::VARIABLE_AFFECTATION);
	SKALANG_GENERATOR_OPERATOR_DEFINE(Operator::VARIABLE_DECLARATION);
}
