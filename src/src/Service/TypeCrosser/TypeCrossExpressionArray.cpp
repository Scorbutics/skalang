#include <sstream>
#include "TypeCrossExpressionArray.h"

ska::Type ska::TypeCrossExpression<ska::ExpressionType::ARRAY>::cross(const std::string& op, const Type& type1, const Type& type2) const {
	//Handles array deleting cells
	if(op == "-" && type1 == ExpressionType::ARRAY) {
		//Either delete a range of values or an index
		if (type2 == ExpressionType::INT) {
			return type1;
		}
		
		if(type2 == ExpressionType::ARRAY) {
			if (type2[0] != ExpressionType::INT) {
				auto ss = std::stringstream{};
				ss << "Unable to use operator \"" << op << "\" on types \"" << type1 << "\" and \"" << type2 << "\"";
				throw std::runtime_error(ss.str());
			}
			return type1;
		}
	}
	
	const auto checkArraySubTypeCoherence = (type1 == type2) || (type2.size() > 0 ? type2[0] == type1 : (type1.size() > 0 && type1[0] == type2));
	if (!checkArraySubTypeCoherence) {
		auto ss = std::stringstream{};
		ss << "Unable to use operator \"" << op << "\" on types \"" << type1 << "\" and \"" << type2 << "\"";
		throw std::runtime_error(ss.str());
	}

	return type2.size() > 0 ? type2 : type1;
}
