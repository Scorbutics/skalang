#include "NodeValue/AST.h"
#include "NodeValue/LogicalOperator.h"
#include "Interpreter.h"
#include "InterpreterOperatorBinary.h"

namespace ska {

	template<class FirstType, class SecondType, class MathComputer, class MathComputer2, class MathComputer3>
	NodeValue ComputeTwoTypeOperation(
		NodeValue firstValue,
		NodeValue secondValue,
		const Type& firstType,
		const Type& secondType,
		MathComputer computer,
		MathComputer2 computer2,
		MathComputer3 computer3) {
		auto firstIsTypeOne = firstType == ExpressionTypeFromNative<FirstType>::value;
		if (firstIsTypeOne) {
			return computer(firstValue.nodeval<FirstType>(), secondValue.nodeval<SecondType>());
		}

		auto secondIsTypeOne = secondType == ExpressionTypeFromNative<FirstType>::value;
		if (secondIsTypeOne) {
			return computer2(firstValue.nodeval<SecondType>(), secondValue.nodeval<FirstType>());
		}
		return computer3(firstValue.nodeval<SecondType>(), secondValue.nodeval<SecondType>());
	}

	NodeValue InterpretMathematicPlus(NodeValue firstValue, NodeValue secondValue, const Type& firstType, const Type& secondType, const Type& destinationType) {
		switch (destinationType.type()) {
		case ExpressionType::STRING: {
			return ComputeTwoTypeOperation<int, std::string>(std::move(firstValue), std::move(secondValue), firstType, secondType,
			[](int t1, const std::string& t2) {
				return std::to_string(t1) + t2;
			}, [](const std::string& t1, int t2) {
				return t1 + std::to_string(t2);
			}, [](const std::string& t1, const std::string& t2) {
				return t1 + t2;
			});
		}

		case ExpressionType::INT:
			return firstValue.nodeval<int>() + secondValue.nodeval<int>();

		case ExpressionType::FLOAT: {
			return ComputeTwoTypeOperation<int, double>(std::move(firstValue), std::move(secondValue), firstType, secondType,
			[](int t1, double t2) {
				return t1 + t2;
			}, [](double t1, int t2) {
				return t1 + t2;
			}, [](double t1, double t2) {
				return t1 + t2;
			});
		}

		default: 
            assert(!"Unhandled math plus operation");
            return "";
		    
	    }
    }

	NodeValue InterpretMathematicMinus(NodeValue firstValue, NodeValue secondValue, const Type& firstType, const Type& secondType, const Type& destinationType) {
		switch (destinationType.type()) {
		case ExpressionType::INT:
			return firstValue.nodeval<int>() - secondValue.nodeval<int>();

		case ExpressionType::FLOAT: {
			return ComputeTwoTypeOperation<int, double>(std::move(firstValue), std::move(secondValue), firstType, secondType,
			[](int t1, double t2) {
				return t1 - t2;
			}, [](double t1, int t2) {
				return t1 - t2;
			}, [](double t1, double t2) {
				return t1 - t2;
			});
		}

		default:
			assert(!"Unhandled math minus operation");
			return "";
		}
	}

	NodeValue InterpretMathematicMultiply(NodeValue firstValue, NodeValue secondValue, const Type& firstType, const Type& secondType, const Type& destinationType) {
		switch (destinationType.type()) {
		case ExpressionType::STRING: {
			return ComputeTwoTypeOperation<int, std::string>(std::move(firstValue), std::move(secondValue), firstType, secondType,
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
			return firstValue.nodeval<int>() * secondValue.nodeval<int>();

		case ExpressionType::FLOAT: {
			return ComputeTwoTypeOperation<int, double>(std::move(firstValue), std::move(secondValue), firstType, secondType,
			[](int t1, double t2) {
				return t1 * t2;
			}, [](double t1, int t2) {
				return t1 * t2;
			}, [](double t1, double t2) {
				return t1 * t2;
			});
		}

		default:
			assert(!"Unhandled math plus operation");
			return "";
		}
	}

	NodeValue InterpretMathematicDivide(NodeValue firstValue, NodeValue secondValue, const Type& firstType, const Type& secondType, const Type& destinationType) {
		switch (destinationType.type()) {
		case ExpressionType::INT:
			if (secondValue.nodeval<int>() == 0) {
				throw std::runtime_error("math error : division by a null value");
			}
			return firstValue.nodeval<int>() / secondValue.nodeval<int>();

		case ExpressionType::FLOAT: {
			return ComputeTwoTypeOperation<int, double>(std::move(firstValue), std::move(secondValue), firstType, secondType,
			[](int t1, double t2) {
				if (t2 == 0) {
					throw std::runtime_error("math error : division by a null value");
				}
				return t1 / t2;
			}, [](double t1, int t2) {
				if (t2 == 0) {
					throw std::runtime_error("math error : division by a null value");
				}
				return t1 / t2;
			}, [](double t1, double t2) {
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

	NodeValue InterpretLogicCondition(NodeValue firstValue, NodeValue secondValue, const Type& firstType, const Type& secondType, const Type& destinationType) {
		assert(destinationType.type() == ExpressionType::BOOLEAN);
        return firstType == secondType && firstValue == secondValue;
    }

	template<class Comparer>
	bool InterpretLogicNumericFromNodeValue(const NodeValue& firstValue, const NodeValue& secondValue, ExpressionType type, Comparer comparison) {
		assert(Type::isNumeric(type));
		switch (type) {
		case ExpressionType::FLOAT:
			return comparison(firstValue.convertNumeric(), secondValue.convertNumeric());
		case ExpressionType::INT:
			return comparison(static_cast<int>(firstValue.convertNumeric()), static_cast<int>(secondValue.convertNumeric()));
		case ExpressionType::BOOLEAN:
			return comparison(static_cast<int>(firstValue.convertNumeric()) == 1, static_cast<int>(secondValue.convertNumeric()) == 1);
		}

		return false;
	}

	NodeValue InterpretLogicLesser(NodeValue firstValue, NodeValue secondValue, ExpressionType destinationType) {
		return InterpretLogicNumericFromNodeValue(firstValue, secondValue, destinationType, [](auto v1, auto v2) {return v1 < v2; });
	}

	NodeValue InterpretLogicLesserOrEqual(NodeValue firstValue, NodeValue secondValue, ExpressionType destinationType) {
		return InterpretLogicNumericFromNodeValue(firstValue, secondValue, destinationType, [](auto v1, auto v2) {return v1 <= v2; });
	}

	NodeValue InterpretLogicGreaterOrEqual(NodeValue firstValue, NodeValue secondValue, ExpressionType destinationType) {
		return InterpretLogicNumericFromNodeValue(firstValue, secondValue, destinationType, [](auto v1, auto v2) {return v1 >= v2; });
	}

	NodeValue InterpretLogicGreater(NodeValue firstValue, NodeValue secondValue, ExpressionType destinationType) {
		return InterpretLogicNumericFromNodeValue(firstValue, secondValue, destinationType, [](auto v1, auto v2) {return v1 > v2; });
	}

	NodeValue InterpretMathematicBinaryExpression(std::string mathOperator, NodeValue firstValue, NodeValue secondValue, const Type& firstType, const Type& secondType, const Type& destinationType) {
		assert(!mathOperator.empty());
		auto operatorIt = LogicalOperatorMap.find(mathOperator);
        if(operatorIt != LogicalOperatorMap.end()) {
            switch (operatorIt->second) {
            case LogicalOperator::ADDITION :
                return InterpretMathematicPlus(std::move(firstValue), std::move(secondValue), firstType, secondType, destinationType);
            case LogicalOperator::SUBSTRACT:
                return InterpretMathematicMinus(std::move(firstValue), std::move(secondValue), firstType, secondType, destinationType);
            case LogicalOperator::MULTIPLY:
                return InterpretMathematicMultiply(std::move(firstValue), std::move(secondValue), firstType, secondType, destinationType);
            case LogicalOperator::DIVIDE:
                return InterpretMathematicDivide(std::move(firstValue), std::move(secondValue), firstType, secondType, destinationType);
            case LogicalOperator::EQUALITY:
                return InterpretLogicCondition(std::move(firstValue), std::move(secondValue), firstType, secondType, destinationType);
			case LogicalOperator::LESSER_OR_EQUAL:
				return InterpretLogicLesserOrEqual(std::move(firstValue), std::move(secondValue), firstType.crossTypes("=", secondType));
			case LogicalOperator::LESSER:
				return InterpretLogicLesser(std::move(firstValue), std::move(secondValue), firstType.crossTypes("=", secondType));
			case LogicalOperator::GREATER_OR_EQUAL:
				return InterpretLogicGreaterOrEqual(std::move(firstValue), std::move(secondValue), firstType.crossTypes("=", secondType));
			case LogicalOperator::GREATER:
				return InterpretLogicGreater(std::move(firstValue), std::move(secondValue), firstType.crossTypes("=", secondType));
            default:
                throw std::runtime_error("Unhandled operator " + mathOperator);
                return "";
            }
	    }
		throw std::runtime_error("Unhandled operator " + mathOperator);
    }
}

ska::NodeCell ska::InterpreterOperator<ska::Operator::BINARY>::interpret(OperateOn node) {
	auto firstValue = m_interpreter.interpret({ node.parent , node.GetFirstValue() }).asRvalue();
	auto secondValue = m_interpreter.interpret({ node.parent , node.GetSecondValue() }).asRvalue();
	auto mathOperator = node.GetOperator();
	return NodeRValue{ InterpretMathematicBinaryExpression(
			std::move(mathOperator),
			std::move(firstValue.object),
			std::move(secondValue.object),
			node.GetFirstValue().type().value(),
			node.GetSecondValue().type().value(),
			node.GetType().value()) };
}

