#include <string>
#include "GeneratorOperatorVariable.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScript.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::bytecode::GeneratorOperator<ska::Operator::VARIABLE_DECLARATION>);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::GeneratorOperator<ska::Operator::VARIABLE_DECLARATION>)

namespace ska {
	namespace bytecode {
		template <class Generator>
		static ScriptGenerationOutput CommonGenerate(Generator& generator, const ASTNode& dest, const ska::ASTNode& node, GenerationContext& context) {
			auto valueGroup = generator.generateNext({ context, node });
			if((dest.symbol() != node.symbol() || node.symbol() == nullptr) && !valueGroup.empty()) {
				LOG_DEBUG << "Creating MOV instruction with value " << valueGroup;
				valueGroup.push(Instruction { Command::MOV, context.querySymbolOrValue(dest), valueGroup.value() });
				LOG_DEBUG << "\tin value " << valueGroup.value();
			}
			return valueGroup;
		}
	}
}

ska::bytecode::ScriptGenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::VARIABLE_AFFECTATION>::generate(OperateOn node, GenerationContext& context) {
	return CommonGenerate(*this, node.GetVariableNameNode(), node.GetVariableValueNode(), context);
}

ska::bytecode::ScriptGenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::VARIABLE_DECLARATION>::generate(OperateOn node, GenerationContext& context) {
	return CommonGenerate(*this, node.GetVariableNameNode(), node.GetVariableValueNode(), context);
}
