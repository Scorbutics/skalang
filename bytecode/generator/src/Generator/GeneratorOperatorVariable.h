#pragma once

#include "GeneratorOperator.h"
#include "Operation/OperationTypeVariableAffectation.h"
#include "Operation/OperationTypeVariableDeclaration.h"

namespace ska {
	namespace bytecode {
		SKALANG_GENERATOR_OPERATOR_DEFINE(Operator::VARIABLE_AFFECTATION);
		SKALANG_GENERATOR_OPERATOR_DEFINE(Operator::VARIABLE_DECLARATION);
	}
}