#pragma once

#include "Operator.h"

namespace ska {
	struct OperatorTraits {

		static constexpr bool isNamed(Operator o) {
			switch (o) {
				case Operator::FUNCTION_DECLARATION:
				case Operator::VARIABLE_DECLARATION:
				case Operator::PARAMETER_DECLARATION:
					return true;
				default:
					return false;
			}
		}
	};
}