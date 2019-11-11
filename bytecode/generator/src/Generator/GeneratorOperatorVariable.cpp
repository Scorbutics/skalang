#include <string>
#include "GeneratorOperatorVariable.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScriptGenerationService.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::GeneratorOperator<ska::Operator::VARIABLE_DECLARATION>);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::GeneratorOperator<ska::Operator::VARIABLE_DECLARATION>)

namespace ska {
	namespace bytecode {
		template <class Generator>
		static ScriptGenerationOutput CommonGenerate(Generator& generator, const ASTNode& dest, const ska::ASTNode& node, GenerationContext& context) {
			auto finalGroup = generator.generateNext({ context, node });
			auto valueDestination = finalGroup.value();
			if((dest.symbol() != node.symbol() || node.symbol() == nullptr) && !finalGroup.empty()) {
				LOG_DEBUG << "Creating MOV instruction with value " << finalGroup;
				auto variable = dest.symbol() != nullptr ? ScriptGenerationOutput{context.querySymbolOrValue(dest)} : generator.generateNext({ context, dest });
				auto variableDestination = variable.value();

				finalGroup.push(std::move(variable));
				finalGroup.push(Instruction { Command::MOV, variableDestination, valueDestination });
				LOG_DEBUG << "\tin value " << valueDestination;
			}
			return finalGroup;
		}
	}
}

ska::bytecode::ScriptGenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::VARIABLE_AFFECTATION>::generate(OperateOn node, GenerationContext& context) {
	return CommonGenerate(*this, node.GetVariableNameNode(), node.GetVariableValueNode(), context);
}

ska::bytecode::ScriptGenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::VARIABLE_DECLARATION>::generate(OperateOn node, GenerationContext& context) {
	return CommonGenerate(*this, node.GetVariableNameNode(), node.GetVariableValueNode(), context);
}
