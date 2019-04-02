#include <sstream>
#include "TypeCrossExpressionArray.h"

ska::Type ska::TypeCrossExpression<ska::ExpressionType::ARRAY>::cross(const std::string& op, const Type& type1, const Type& type2) const {
	const auto checkArraySubTypeCoherence = (type1 == type2) || (type2.size() > 0 ? type2.compound()[0] == type1 : (type1.size() > 0 && type1.compound()[0] == type2));
	if (!checkArraySubTypeCoherence) {
		auto ss = std::stringstream{};
		ss << "Unable to use operator \"" << op << "\" on types " << type1 << " and " << type2;
		throw std::runtime_error(ss.str());
	}
	return type2.size() > 0 ? type2 : type1;
}
