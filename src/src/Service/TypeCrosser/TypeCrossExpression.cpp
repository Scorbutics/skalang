#include "TypeCrossExpression.h"

namespace ska {
	namespace typedetail {
		template <ExpressionType t>
		inline void TypeCrosserMakeCross(TypeCrosser::TypeCrossMap& result) {
			result[static_cast<std::size_t>(t)] = std::make_unique<TypeCrossExpression<t>>();
		}
	}
}

ska::TypeCrosser::TypeCrossMap ska::TypeCrosser::typeCrossMap = ska::TypeCrosser::BuildTypeCrossMap();

ska::TypeCrosser::TypeCrossMap ska::TypeCrosser::BuildTypeCrossMap() {
	auto result = TypeCrossMap{};
	typedetail::TypeCrosserMakeCross<ExpressionType::ARRAY>(result);
	typedetail::TypeCrosserMakeCross<ExpressionType::OBJECT>(result);
	return result;
}

ska::Type ska::TypeCrosser::cross(const std::string& op, const Type& type1, const Type& type2) const {
	const auto basicComputedExpressionType = typedetail::ExpressionTypeCross(op, type1.type(), type2.type());
	const auto& complexTypeCrosser = typeCrossMap[static_cast<std::size_t>(basicComputedExpressionType)];
	if(complexTypeCrosser != nullptr) {
		return complexTypeCrosser->cross(op, type1, type2);
	}
	return Type{ basicComputedExpressionType };
}
