#include <cassert>
#include <sstream>
#include "InterpretLogic.h"
#include "NodeValue/Type.h"

namespace ska {
	template<class Comparer>
	bool InterpretLogicNumericFromNodeValue(const NodeValue& firstValue, const NodeValue& secondValue, const Type& type, Comparer comparison) {
		assert(Type::isNumeric(type));
		switch (type.type()) {
		case ExpressionType::FLOAT:
			return comparison(firstValue.convertNumeric(), secondValue.convertNumeric());
		case ExpressionType::INT:
			return comparison(static_cast<int>(firstValue.convertNumeric()), static_cast<int>(secondValue.convertNumeric()));
		case ExpressionType::BOOLEAN:
			return comparison(static_cast<int>(firstValue.convertNumeric()) == 1, static_cast<int>(secondValue.convertNumeric()) == 1);
		default:
		{
			auto ss = std::stringstream{};
			ss << "unhandled type " << type << " for comparison";
			throw std::runtime_error(ss.str());
		}
		}

		return false;
	}
}

ska::NodeValue ska::InterpretLogicLesser(TypedNodeValue firstValue, TypedNodeValue secondValue, const Type& destinationType) {
	return InterpretLogicNumericFromNodeValue(firstValue.value, secondValue.value, destinationType, [](auto v1, auto v2) {return v1 < v2; });
}

ska::NodeValue ska::InterpretLogicLesserOrEqual(TypedNodeValue firstValue, TypedNodeValue secondValue, const Type& destinationType) {
	return InterpretLogicNumericFromNodeValue(firstValue.value, secondValue.value, destinationType, [](auto v1, auto v2) {return v1 <= v2; });
}

ska::NodeValue ska::InterpretLogicGreaterOrEqual(TypedNodeValue firstValue, TypedNodeValue secondValue, const Type& destinationType) {
	return InterpretLogicNumericFromNodeValue(firstValue.value, secondValue.value, destinationType, [](auto v1, auto v2) {return v1 >= v2; });
}

ska::NodeValue ska::InterpretLogicGreater(TypedNodeValue firstValue, TypedNodeValue secondValue, const Type& destinationType) {
	return InterpretLogicNumericFromNodeValue(firstValue.value, secondValue.value, destinationType, [](auto v1, auto v2) {return v1 > v2; });
}

ska::NodeValue ska::InterpretLogicCondition(TypedNodeValue firstValue, TypedNodeValue secondValue, const Type& destinationType) {
	assert(destinationType.type() == ExpressionType::BOOLEAN);
	return firstValue.type == secondValue.type && firstValue.value == secondValue.value;
}
