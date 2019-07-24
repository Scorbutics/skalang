#include "Config/LoggerConfigLang.h"
#include "Base/SkaConstants.h"
#include "NodeValue/AST.h"
#include "NodeValue/LogicalOperator.h"
#include "GeneratorOperatorBinary.h"
#include "Interpreter/Value/TypedNodeValue.h"
#include "Generator/Value/BytecodeScript.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::GeneratorOperator<ska::Operator::FUNCTION_CALL>);

#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::GeneratorOperator<ska::Operator::FUNCTION_CALL>)

namespace ska {

	static BytecodeCell GenerateMathematicBinaryExpression(std::string mathOperator, const BytecodeCell& second, const Type& destinationType) {
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
	//create a temporary variable (left operand)
	auto leftGroup = m_generator.generate({ context.script(), node.GetFirstNode() });

	//create a temporary variable (right operand)
	auto rightGroup = m_generator.generate({ context.script(), node.GetSecondNode() });

	LOG_DEBUG << "Creating left tmp group \"" << leftGroup << "\"";
	auto leftVariable = context.script().package(leftGroup);
	assert(leftVariable.has_value());
#ifdef SKA_DEBUG_LOGS
	if(leftGroup.size() >= 1) {
		LOG_DEBUG << "\t in variable \"" << *leftVariable << "\"";
	}
#endif
	
	LOG_DEBUG << "Creating right tmp group \"" << rightGroup << "\"";
	auto rightVariable = context.script().package(rightGroup);
	assert(rightVariable.has_value());
#ifdef SKA_DEBUG_LOGS
	if(rightGroup.size() >= 1) {
		LOG_DEBUG << "\t in variable \"" << *rightVariable << "\"";
	}
#endif

	auto mathOperator = node.GetOperator();
	auto operationValue = GenerateMathematicBinaryExpression(
		std::move(mathOperator),
		*rightVariable,
		node.asNode().type().value());

	auto result = BytecodeCellGroup{};
	
	//It is important to put the right group before the left one : the ast order has to be reversed
	//TODO move instead of copy
	result.insert(result.end(), rightGroup.begin(), rightGroup.end());
	result.insert(result.end(), leftGroup.begin(), leftGroup.end());
	result.push_back(*leftVariable);
	result.push_back(std::move(operationValue));
	
	LOG_DEBUG << "Result \"" << result << "\"\n";

	return result;
}
