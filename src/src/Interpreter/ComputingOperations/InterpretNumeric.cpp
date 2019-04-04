#include "InterpretNumeric.h"
#include "ComputeTwoTypeOperation.h"

template <>
std::string ska::nodeval_to_string<std::string>(const std::string& obj) {
	return obj;
}

ska::NodeValue ska::InterpretMathematicPlus(TypedNodeValue firstValue, TypedNodeValue secondValue, const Type& destinationType) {
	switch (destinationType.type()) {
	case ExpressionType::STRING: {
		return ComputeTwoTypeOperation<int, std::string>(std::move(firstValue), std::move(secondValue),
		[](const auto& t1, const auto& t2) {
			return nodeval_to_string(t1) + nodeval_to_string(t2);
		});
	}

	case ExpressionType::INT:
		return firstValue.value.nodeval<int>() + secondValue.value.nodeval<int>();

	case ExpressionType::FLOAT: {
		return ComputeTwoTypeOperation<int, double>(std::move(firstValue), std::move(secondValue),
		[](const auto & t1, const auto & t2) {
			return t1 + t2;
		});
	}
	case ExpressionType::ARRAY: {
		auto lambdas = std::make_tuple([](auto& t1, NodeValueArray& t2) {
			t2->push_back(std::move(t1));
			return t2;
		}, [](NodeValueArray& t1, auto& t2) {
			t1->push_back(std::move(t2));
			return t1;
		}, [](NodeValueArray& t1, NodeValueArray& t2) {
			t1->insert(t1->end(), std::make_move_iterator(t2->begin()), std::make_move_iterator(t2->end()));
			return t1;
		});

		switch (firstValue.type.compound()[0].type()) {
		case ExpressionType::STRING:
			return ComputeTwoTypeOperation<std::string, NodeValueArray>(std::move(firstValue), std::move(secondValue),
			std::get<0>(lambdas),
			std::get<1>(lambdas),
			std::get<2>(lambdas)
			);
		case ExpressionType::INT:
			return ComputeTwoTypeOperation<int, NodeValueArray>(std::move(firstValue), std::move(secondValue),
			std::get<0>(lambdas),
			std::get<1>(lambdas),
			std::get<2>(lambdas));
		case ExpressionType::FLOAT:
			return ComputeTwoTypeOperation<double, NodeValueArray>(std::move(firstValue), std::move(secondValue),
			std::get<0>(lambdas),
			std::get<1>(lambdas),
			std::get<2>(lambdas));
		}
		return "";
	}

	default: 
		throw std::runtime_error("Unhandled math plus operation");
		return "";
		
	}
}

ska::NodeValue ska::InterpretMathematicMinus(TypedNodeValue firstValue, TypedNodeValue secondValue, const Type& destinationType) {
	switch (destinationType.type()) {
	case ExpressionType::INT:
		return firstValue.value.nodeval<int>() - secondValue.value.nodeval<int>();

	case ExpressionType::FLOAT: {
		return ComputeTwoTypeOperation<int, double>(std::move(firstValue), std::move(secondValue),
		[](const auto& t1, const auto& t2) {
			return t1 + t2;
		});
	}

	default:
		throw std::runtime_error("Unhandled math minus operation");
		return "";
	}
}

ska::NodeValue ska::InterpretMathematicMultiply(TypedNodeValue firstValue, TypedNodeValue secondValue, const Type& destinationType) {
	switch (destinationType.type()) {
	case ExpressionType::STRING: {
		return ComputeTwoTypeOperation<int, std::string>(std::move(firstValue), std::move(secondValue),
		[](int t1, const std::string& t2) {
			assert(t1 > 0);
			auto ss = std::stringstream{};
			for (auto i = 0u; i < static_cast<std::size_t>(t1); i++) {
				ss << t2;
			}
			return ss.str();
		}, [](const std::string& t2, int t1) {
			assert(t1 > 0);
			auto ss = std::stringstream{};
			for (auto i = 0u; i < static_cast<std::size_t>(t1); i++) {
				ss << t2;
			}
			return ss.str();
		}, [](const std::string& t1, const std::string& t2) {
			throw std::runtime_error("unhandled case : string multiply by string");
			return "";
		});
	}

	case ExpressionType::INT:
		return firstValue.value.nodeval<int>() * secondValue.value.nodeval<int>();

	case ExpressionType::FLOAT: {
		return ComputeTwoTypeOperation<int, double>(std::move(firstValue), std::move(secondValue),
		[](const auto& t1, const auto& t2) {
			return t1 * t2;
		});
	}

	default:
		assert(!"Unhandled math plus operation");
		return "";
	}
}

ska::NodeValue ska::InterpretMathematicDivide(TypedNodeValue firstValue, TypedNodeValue secondValue, const Type& destinationType) {
	switch (destinationType.type()) {
	case ExpressionType::INT:
		if (secondValue.value.nodeval<int>() == 0) {
			throw std::runtime_error("math error : division by a null value");
		}
		return firstValue.value.nodeval<int>() / secondValue.value.nodeval<int>();

	case ExpressionType::FLOAT: {
		return ComputeTwoTypeOperation<int, double>(std::move(firstValue), std::move(secondValue),
		[](const auto& t1, const auto& t2) {
			if (t2 == 0) {
				throw std::runtime_error("math error : division by a null value");
			}
			return t1 / t2;
		});
	}

	default:
		assert(!"Unhandled math plus operation");
		return "";
	}
}
