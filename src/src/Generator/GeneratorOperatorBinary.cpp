#include "NodeValue/AST.h"
#include "NodeValue/LogicalOperator.h"
#include "GeneratorOperatorBinary.h"
#include "Interpreter/Value/TypedNodeValue.h"
#include "Generator/Value/BytecodeScript.h"

namespace ska {

	static BytecodeCell GenerateMathematicBinaryExpression(std::string mathOperator, const BytecodeCell& first, const BytecodeCell& second, const Type& destinationType) {
		assert(!mathOperator.empty());
		auto operatorIt = LogicalOperatorMap.find(mathOperator);
        if(operatorIt != LogicalOperatorMap.end()) {
            switch (operatorIt->second) {
            case LogicalOperator::ADDITION :
				return { BytecodeCommand::ADD, destinationType, second.value() };
				//GenerateMathematicPlus(std::move(firstValue), std::move(secondValue), destinationType);
            case LogicalOperator::SUBSTRACT:
				return { BytecodeCommand::SUB, destinationType, second.value() };
                //return GenerateMathematicMinus(std::move(firstValue), std::move(secondValue), destinationType);
            case LogicalOperator::MULTIPLY:
				return { BytecodeCommand::MUL, destinationType, second.value() };
                //return GenerateMathematicMultiply(std::move(firstValue), std::move(secondValue), destinationType);
            case LogicalOperator::DIVIDE:
				return { BytecodeCommand::DIV, destinationType, second.value() };
                //return GenerateMathematicDivide(std::move(firstValue), std::move(secondValue), destinationType);
            
			/*
			case LogicalOperator::EQUALITY:
                return GenerateLogicCondition(std::move(firstValue), std::move(secondValue), destinationType);
			
			case LogicalOperator::LESSER_OR_EQUAL: {
				const auto crossedType = firstValue.type.crossTypes(crosser, "=", secondValue.type);
				return GenerateLogicLesserOrEqual(std::move(firstValue), std::move(secondValue), crossedType);
			}
			case LogicalOperator::LESSER: {
				const auto crossedType = firstValue.type.crossTypes(crosser, "=", secondValue.type);
				return GenerateLogicLesser(std::move(firstValue), std::move(secondValue), crossedType);
			}
			case LogicalOperator::GREATER_OR_EQUAL: {
				const auto crossedType = firstValue.type.crossTypes(crosser, "=", secondValue.type);
				return GenerateLogicGreaterOrEqual(std::move(firstValue), std::move(secondValue), crossedType);
			}
			case LogicalOperator::GREATER: {
				const auto crossedType = firstValue.type.crossTypes(crosser, "=", secondValue.type);
				return GenerateLogicGreater(std::move(firstValue), std::move(secondValue), crossedType);
			}
			*/
            default:
				break;
            }
	    }
		throw std::runtime_error("Unhandled operator " + mathOperator);
    }
}

ska::BytecodeCellGroup ska::GeneratorOperator<ska::Operator::BINARY>::generate(OperateOn node, BytecodeGenerationContext& context) {
	auto firstValue = m_generator.generate({ context.script(), node.GetFirstNode() });
	
	//create a temporary variable (left operand)
	if (firstValue.size() > 1) {
		auto tmpGroup = context.script().newGroup(firstValue);
		firstValue.push_back(firstValue[0].toInCell(std::move(tmpGroup)));
	}

	auto secondValue = m_generator.generate({ context.script(), node.GetSecondNode() });

	//create a temporary variable (right operand)
	if (secondValue.size() > 1) {
		auto tmpGroup = context.script().newGroup(secondValue);
		secondValue.push_back(secondValue[0].toInCell(std::move(tmpGroup)));
	}

	auto mathOperator = node.GetOperator();
	auto resultValue = GenerateMathematicBinaryExpression(
		std::move(mathOperator),
		firstValue.back(),
		secondValue.back(),
		node.asNode().type().value());

	auto result = BytecodeCellGroup{};
	//TODO move instead of copy
	result.insert(result.end(), firstValue.begin(), firstValue.end());
	result.push_back(std::move(resultValue));
	//result.insert(result.end(), secondValue.begin(), secondValue.end());
	
	return result;
}

/*
ska::GeneratorOperator<ska::Operator::BINARY>::GeneratorOperator(BytecodeGenerator& generator, const TypeCrosser& typeCrosser) :
	ska::GeneratorOperatorBase(generator),
	m_typeCrosser(typeCrosser) {
}
*/
