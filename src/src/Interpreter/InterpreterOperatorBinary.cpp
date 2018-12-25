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
			return computer(nodeval<FirstType>(firstValue), nodeval<SecondType>(secondValue));
		}

		auto secondIsTypeOne = secondType == ExpressionTypeFromNative<FirstType>::value;
		if (secondIsTypeOne) {
			return computer2(nodeval<SecondType>(firstValue), nodeval<FirstType>(secondValue));
		}
		return computer3(nodeval<SecondType>(firstValue), nodeval<SecondType>(secondValue));
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
			return nodeval<int>(firstValue) + nodeval<int>(secondValue);

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
			return nodeval<int>(firstValue) - nodeval<int>(secondValue);

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
			return nodeval<int>(firstValue) * nodeval<int>(secondValue);

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
			if (nodeval<int>(secondValue) == 0) {
				throw std::runtime_error("math error : division by a null value");
			}
			return nodeval<int>(firstValue) / nodeval<int>(secondValue);

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
            case LogicalOperator::CONDITION:
                return InterpretLogicCondition(std::move(firstValue), std::move(secondValue), firstType, secondType, destinationType);
            default:
                throw std::runtime_error("Unhandled operator " + mathOperator);
                return "";
            }
	    }
    }
}

ska::NodeValue ska::InterpreterOperator<ska::Operator::BINARY>::interpret(const SymbolTable& symbols, MemoryTable& memory, ASTNode& node) {
	auto firstValue = m_interpreter.interpret(node[0]);
	auto secondValue = m_interpreter.interpret(node[1]);
	auto mathOperator = node.name();
	return InterpretMathematicBinaryExpression(
			std::move(mathOperator), 
			std::move(firstValue), 
			std::move(secondValue), 
			node[0].type().value(), 
			node[1].type().value(),
			node.type().value());
}

