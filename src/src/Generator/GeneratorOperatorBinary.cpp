#include "Config/LoggerConfigLang.h"
#include "Base/SkaConstants.h"
#include "NodeValue/AST.h"
#include "GeneratorOperatorBinary.h"
#include "Interpreter/Value/TypedNodeValue.h"
#include "Generator/Value/BytecodeScript.h"
#include "ComputingOperations/BytecodeTypeConversion.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::GeneratorOperator<ska::Operator::BINARY>);

#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::GeneratorOperator<ska::Operator::BINARY>)

namespace ska {
	namespace bytecode {
		static Instruction GenerateMathematicBinaryExpression(std::string logicalOperator, Register dest, Value left, Value right) {
			assert(!logicalOperator.empty());
			auto operatorIt = CommandMap.find(logicalOperator);
			if(operatorIt != CommandMap.end()) {
				return { operatorIt->second, std::move(dest), std::move(left), std::move(right) };
			}
			throw std::runtime_error("Unhandled operator " + logicalOperator);
		}

		static GenerationOutput GenerateInstructionValue(Generator& generator, Script& script, const ASTNode& parent, const ASTNode& node) {
			if(node.size() == 0) {
				return script.queryVariableOrValue(node);
			}
			return generator.generate({ script, node });
		}
	}
}

ska::bytecode::GenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::BINARY>::generate(OperateOn node, GenerationContext& context) {
	auto logicalOperator = LogicalOperatorMap.at(node.GetOperator());

	auto children = std::vector<const ASTNode*> { &node.GetFirstNode(), &node.GetSecondNode() };
	auto groups = std::vector<GenerationOutput>{};

	for(const auto* child : children) {
		groups.push_back(InstructionPack{});
		auto group = GenerateInstructionValue(m_generator, context.script(), node.asNode(), *child);

		auto conversionGroup = TypeConversion(logicalOperator, group.value(), child->type().value(), node.asNode().type().value());
		if(conversionGroup.has_value()) {
			groups.back().push(std::move(conversionGroup.value()));
		}

		groups.back().push(std::move(group));
	}

	auto currentRegister = context.script().queryNextRegister(node.asNode().type().value());

	auto operationValue = GenerateMathematicBinaryExpression(
		node.GetOperator(),
		std::move(currentRegister),
		groups[0].value(),
		groups[1].value()
	);

	auto result = GenerationOutput{ std::move(groups[0]) };
	result.push(std::move(groups[1]));
	result.push( InstructionPack{ std::move(operationValue) } );

	//It is important to put the right group before the left one : the ast order has to be reversed
	LOG_DEBUG << "Result \"" << result.pack() << "\"\n";

	return result;
}
