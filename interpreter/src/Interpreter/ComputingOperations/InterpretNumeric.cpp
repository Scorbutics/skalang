#include "InterpretNumeric.h"
#include "ComputeTwoTypeOperation.h"

template <>
std::string ska::nodeval_to_string<ska::StringShared>(const StringShared& obj) {
	return obj == nullptr ? "" : *obj;
}

ska::NodeValue ska::InterpretMathematicPlus(TypedNodeValue firstValue, TypedNodeValue secondValue, const Type& destinationType) {
	switch (destinationType.type()) {
	case ExpressionType::STRING: {
		return ComputeTwoTypeOperation<long, StringShared>(std::move(firstValue), std::move(secondValue),
		[](const auto& t1, const auto& t2) {
			return std::make_shared<std::string>((nodeval_to_string(t1) + nodeval_to_string(t2)));
		});
	}

	case ExpressionType::INT:
		return firstValue.value.nodeval<long>() + secondValue.value.nodeval<long>();

	case ExpressionType::FLOAT: {
		return ComputeTwoTypeOperation<long, double>(std::move(firstValue), std::move(secondValue),
		[](const auto & t1, const auto & t2) {
			return t1 + t2;
		});
	}
	case ExpressionType::ARRAY: {
		auto lambdas = std::make_tuple([](auto& t1, NodeValueArray& t2) {
			t2->push_front(std::move(t1));
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
			return ComputeTwoTypeOperation<StringShared, NodeValueArray>(std::move(firstValue), std::move(secondValue),
			std::get<0>(lambdas),
			std::get<1>(lambdas),
			std::get<2>(lambdas)
			);
		case ExpressionType::INT:
			return ComputeTwoTypeOperation<long, NodeValueArray>(std::move(firstValue), std::move(secondValue),
			std::get<0>(lambdas),
			std::get<1>(lambdas),
			std::get<2>(lambdas));
		case ExpressionType::FLOAT:
			return ComputeTwoTypeOperation<double, NodeValueArray>(std::move(firstValue), std::move(secondValue),
			std::get<0>(lambdas),
			std::get<1>(lambdas),
			std::get<2>(lambdas));
		default:
        	throw std::runtime_error("Unhandled math plus operation result");
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
		return firstValue.value.nodeval<long>() - secondValue.value.nodeval<long>();

	case ExpressionType::FLOAT: {
		return ComputeTwoTypeOperation<long, double>(std::move(firstValue), std::move(secondValue),
		[](const auto& t1, const auto& t2) {
			return t1 + t2;
		});
	}

	case ExpressionType::ARRAY: {
		auto lambdas = std::make_tuple([](long t1, NodeValueArray& t2) {
			throw std::runtime_error("Unhandled math minus operation");
			return t2;
		}, [](NodeValueArray& t1, long t2) {
			if(t2 < 0) {
				throw std::runtime_error("Bad vector-based index deletion : negative index(es)");
			}
			if (t1->size() <= t2) {
				throw std::runtime_error("Bad vector-based index deletion : out of bound");
			}

			t1->resize(t1->size() - static_cast<std::size_t>(t2));
			return t1;
		}, [](NodeValueArray& t1, NodeValueArray& t2) { 
			throw std::runtime_error("Unhandled math minus operation"); 
			return t1; 
		} );

		switch (secondValue.type.type()) {
		case ExpressionType::INT:
			return ComputeTwoTypeOperation<long, NodeValueArray>(std::move(firstValue), std::move(secondValue),
			std::get<0>(lambdas),
			std::get<1>(lambdas),
			std::get<2>(lambdas));
		case ExpressionType::ARRAY: {
			auto& t1 = firstValue.value.nodeval<NodeValueArray>();
			auto& t2 = secondValue.value.nodeval<NodeValueArray>();
			if(t2->size() != 2) {
				throw std::runtime_error("Bad vector-based index deletion : size != 2");
			}
			if((*t2)[0].nodeval<long>() < 0 || (*t2)[1].nodeval<long>() < 0) {
				throw std::runtime_error("Bad vector-based index deletion : negative index(es)");
			}
			auto start = static_cast<std::size_t>((*t2)[0].nodeval<long>());
			auto end = static_cast<std::size_t>((*t2)[1].nodeval<long>());
			if(start > end) {
				std::swap(start, end);
			}
			if (t1->size() <= end) {
				throw std::runtime_error("Bad vector-based index deletion : out of bound");
			}

			t1->erase(t1->begin() + start , t1->begin() + end + 1);
			return t1;
		}
		default: {
			auto ss = std::stringstream {};
			ss << "unhandled math minus operation between an array and " << secondValue.type;
			throw std::runtime_error(ss.str());
		}
		}
		return "";
	}
	default:
		throw std::runtime_error("Unhandled math minus operation");
		return "";
	}
}

ska::NodeValue ska::InterpretMathematicMultiply(TypedNodeValue firstValue, TypedNodeValue secondValue, const Type& destinationType) {
	switch (destinationType.type()) {
	case ExpressionType::STRING: {
		return ComputeTwoTypeOperation<long, StringShared>(std::move(firstValue), std::move(secondValue),
		[](int t1, const StringShared& t2) {
			assert(t1 > 0);
			auto ss = std::stringstream{};
			for (auto i = 0u; i < static_cast<std::size_t>(t1); i++) {
				ss << (*t2);
			}
			return std::make_shared<std::string>(ss.str());
		}, [](const StringShared& t2, int t1) {
			assert(t1 > 0);
			auto ss = std::stringstream{};
			for (auto i = 0u; i < static_cast<std::size_t>(t1); i++) {
				ss << (*t2);
			}
			return std::make_shared<std::string>(ss.str());
		}, [](const StringShared& t1, const StringShared& t2) {
			throw std::runtime_error("unhandled case : string multiply by string");
			return "";
		});
	}

	case ExpressionType::INT:
		return firstValue.value.nodeval<long>() * secondValue.value.nodeval<long>();

	case ExpressionType::FLOAT: {
		return ComputeTwoTypeOperation<long, double>(std::move(firstValue), std::move(secondValue),
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
		if (secondValue.value.nodeval<long>() == 0) {
			throw std::runtime_error("math error : division by a null value");
		}
		return firstValue.value.nodeval<long>() / secondValue.value.nodeval<long>();

	case ExpressionType::FLOAT: {
		return ComputeTwoTypeOperation<long, double>(std::move(firstValue), std::move(secondValue),
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
