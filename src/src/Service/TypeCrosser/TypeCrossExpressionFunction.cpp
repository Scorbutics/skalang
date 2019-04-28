#include "TypeCrossExpressionFunction.h"

ska::Type ska::TypeCrossExpression<ska::ExpressionType::FUNCTION>::cross(const std::string& op, const Type& type1, const Type& type2) const {
	if (op == "=") {
		if (type1 != type2) {
			return Type{};
		}
		return type1;
	}
	return Type{};
}