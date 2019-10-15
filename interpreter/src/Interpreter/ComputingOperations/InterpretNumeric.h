#pragma once
#include "Interpreter/Value/TypedNodeValue.h"

namespace ska {
	NodeValue InterpretMathematicPlus(TypedNodeValue firstValue, TypedNodeValue secondValue, const Type& destinationType);
	NodeValue InterpretMathematicMinus(TypedNodeValue firstValue, TypedNodeValue secondValue, const Type& destinationType);
	NodeValue InterpretMathematicMultiply(TypedNodeValue firstValue, TypedNodeValue secondValue, const Type& destinationType);
	NodeValue InterpretMathematicDivide(TypedNodeValue firstValue, TypedNodeValue secondValue, const Type& destinationType);

	template <typename T>
	std::string nodeval_to_string(const T& obj) {
		return std::to_string(obj);
	}

	template <>
	std::string nodeval_to_string<StringShared>(const StringShared& obj);
}
