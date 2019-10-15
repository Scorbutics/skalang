#pragma once
#include "Interpreter/Value/TypedNodeValue.h"
#include "NodeValue/TypeFromNative.h"

namespace ska {
	template<class SimpleType, class DoubleType, class MathComputerSimple, class MathComputerSimpleInv, class MathComputerDouble>
	NodeValue ComputeTwoTypeOperation(
		TypedNodeValue firstValue,
		TypedNodeValue secondValue,
		MathComputerSimple&& computer,
		MathComputerSimpleInv&& computerInv,
		MathComputerDouble&& computerEx) {
		if (firstValue.type != TypeFromNative<SimpleType>::value() &&
			secondValue.type != TypeFromNative<SimpleType>::value()) {
			return computerEx(firstValue.value.nodeval<DoubleType>(), secondValue.value.nodeval<DoubleType>());
		}

		return firstValue.type == TypeFromNative<SimpleType>::value() ?
			computer(firstValue.value.nodeval<SimpleType>(), secondValue.value.nodeval<DoubleType>()) :
			computerInv(firstValue.value.nodeval<DoubleType>(), secondValue.value.nodeval<SimpleType>());
	}

	template<class SimpleType, class DoubleType, class MathComputerSimple, class MathComputerDouble>
	NodeValue ComputeTwoTypeOperation(
		TypedNodeValue firstValue,
		TypedNodeValue secondValue,
		MathComputerSimple&& computer,
		MathComputerDouble&& computerEx) {
		return ComputeTwoTypeOperation<SimpleType, DoubleType>(std::move(firstValue), std::move(secondValue), computer, computer, computer);
	}

	template<class SimpleType, class DoubleType, class MathComputer>
	NodeValue ComputeTwoTypeOperation(
		TypedNodeValue firstValue,
		TypedNodeValue secondValue,
		MathComputer&& computer) {
		return ComputeTwoTypeOperation<SimpleType, DoubleType>(std::move(firstValue), std::move(secondValue), computer, computer);
	}
}
