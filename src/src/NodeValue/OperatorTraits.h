#pragma once

#include "NodeValue/Operator.h"

namespace ska {
	struct OperatorTraits {

		static constexpr bool isNamed(Operator o) {
			switch (o) {
				case Operator::FUNCTION_PROTOTYPE_DECLARATION:
				case Operator::FUNCTION_DECLARATION:
				case Operator::VARIABLE_AFFECTATION:
				case Operator::PARAMETER_DECLARATION:
					return true;
				default:
					return false;
			}
		}

		static constexpr bool isLvalueCompatible(Operator o) {
			switch (o) {
			case Operator::UNARY:
			case Operator::FIELD_ACCESS:
			case Operator::FUNCTION_DECLARATION:
			case Operator::AFFECTATION:
			case Operator::VARIABLE_AFFECTATION:
			case Operator::ARRAY_USE:
			case Operator::ARRAY_DECLARATION:
				return true;
			default:
				return false;
			}
		}
	};
}