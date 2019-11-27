#include "Config/LoggerConfigLang.h"
#include "Base/SkaConstants.h"
#include "NodeValue/AST.h"
#include "GeneratorOperatorBinary.h"
#include "Generator/Value/BytecodeScriptGenerationHelper.h"
#include "ComputingOperations/BytecodeTypeConversion.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::GeneratorOperator<ska::Operator::BINARY>);

#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::GeneratorOperator<ska::Operator::BINARY>)

namespace ska {
	namespace bytecode {
		static InstructionOutput GenerateMathematicBinaryExpression(std::string logicalOperator, const TypedOperandRef& dest, const TypedOperandRef& left, const TypedOperandRef& right) {
			assert(!logicalOperator.empty());
			auto operatorIt = LogicalOperatorMap.find(logicalOperator);
			if(operatorIt != LogicalOperatorMap.end()) {
				return TypeConversionBinary(
					operatorIt->second,
					left,
					right,
					dest
				);
			}
			throw std::runtime_error("Unhandled operator " + logicalOperator);
		}

		static InstructionOutput GenerateInstructionValue(GeneratorOperator<ska::Operator::BINARY>& generator, GenerationContext& context, const ASTNode& parent, const ASTNode& node) {
			if(node.size() == 0) {
				return context.querySymbolOrOperand(node);
			}
			return generator.generateNext({ context, node });
		}
	}
}

ska::bytecode::InstructionOutput ska::bytecode::GeneratorOperator<ska::Operator::BINARY>::generate(OperateOn node, GenerationContext& context) {
	auto children = std::vector<const ASTNode*> { &node.GetFirstNode(), &node.GetSecondNode() };
	auto groups = std::vector<InstructionOutput>{};

	for(const auto* child : children) {
		auto group = GenerateInstructionValue(*this, context, node.asNode(), *child);
		groups.push_back(std::move(group));
		LOG_DEBUG << "Binary : Value node child " << groups.back().operand().toString();
	}

	auto currentRegister = context.queryNextRegister();

	auto operationValue = GenerateMathematicBinaryExpression(
		node.GetOperator(),
		{node.asNode().type().value(), currentRegister},
		{children[0]->type().value(), groups[0].operand()},
		{children[1]->type().value(), groups[1].operand()}
	);

	auto result = std::move(groups[0]);
	result.push(std::move(groups[1]));
	result.push(std::move(operationValue));

	//It is important to put the right group before the left one : the ast order has to be reversed
	LOG_DEBUG << "Result \"" << result << "\"\n";

	return result;
}
