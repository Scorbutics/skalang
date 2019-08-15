#include "NodeValue/AST.h"
#include "NodeValue/LogicalOperator.h"
#include "InterpreterOperatorBinary.h"
#include "ComputingOperations/InterpretLogic.h"
#include "ComputingOperations/InterpretNumeric.h"
#include "Interpreter/Value/TypedNodeValue.h"
#include "Interpreter.h"

namespace ska {

	NodeValue InterpretMathematicBinaryExpression(const TypeCrosser& crosser, std::string mathOperator, TypedNodeValue firstValue, TypedNodeValue secondValue, const Type& destinationType) {
		assert(!mathOperator.empty());
		auto operatorIt = LogicalOperatorMap.find(mathOperator);
        if(operatorIt != LogicalOperatorMap.end()) {
            switch (operatorIt->second) {
            case LogicalOperator::ADDITION :
                return InterpretMathematicPlus(std::move(firstValue), std::move(secondValue), destinationType);
            case LogicalOperator::SUBSTRACT:
                return InterpretMathematicMinus(std::move(firstValue), std::move(secondValue), destinationType);
            case LogicalOperator::MULTIPLY:
                return InterpretMathematicMultiply(std::move(firstValue), std::move(secondValue), destinationType);
            case LogicalOperator::DIVIDE:
                return InterpretMathematicDivide(std::move(firstValue), std::move(secondValue), destinationType);
            case LogicalOperator::EQUALITY:
                return InterpretLogicCondition(std::move(firstValue), std::move(secondValue), destinationType);
			case LogicalOperator::LESSER_OR_EQUAL: {
				const auto crossedType = firstValue.type.crossTypes(crosser, "=", secondValue.type);
				return InterpretLogicLesserOrEqual(std::move(firstValue), std::move(secondValue), crossedType);
			}
			case LogicalOperator::LESSER: {
				const auto crossedType = firstValue.type.crossTypes(crosser, "=", secondValue.type);
				return InterpretLogicLesser(std::move(firstValue), std::move(secondValue), crossedType);
			}
			case LogicalOperator::GREATER_OR_EQUAL: {
				const auto crossedType = firstValue.type.crossTypes(crosser, "=", secondValue.type);
				return InterpretLogicGreaterOrEqual(std::move(firstValue), std::move(secondValue), crossedType);
			}
			case LogicalOperator::GREATER: {
				const auto crossedType = firstValue.type.crossTypes(crosser, "=", secondValue.type);
				return InterpretLogicGreater(std::move(firstValue), std::move(secondValue), crossedType);
			}
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
			m_typeCrosser,
			std::move(mathOperator),
			TypedNodeValue{ std::move(firstValue.object), node.GetFirstValue().type().value()},
			TypedNodeValue { std::move(secondValue.object), node.GetSecondValue().type().value()},
			node.GetType().value()) };
}

ska::InterpreterOperator<ska::Operator::BINARY>::InterpreterOperator(Interpreter& interpreter, const TypeCrosser& typeCrosser) :
	ska::InterpreterOperatorBase(interpreter),
	m_typeCrosser(typeCrosser) {
}
