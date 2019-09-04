#include <string>
#include "GeneratorOperatorVariable.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScript.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::bytecode::GeneratorOperator<ska::Operator::VARIABLE_DECLARATION>);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::GeneratorOperator<ska::Operator::VARIABLE_DECLARATION>)

namespace ska {
	namespace bytecode {
		template <class Generator>
		static GenerationOutput CommonGenerate(Generator& generator, const ASTNode& dest, const ska::ASTNode& node, GenerationContext& context) {
			auto valueGroup = generator.generateNext({ context.script(), node, context.scope() });
			if((dest.symbol() != node.symbol() || node.symbol() == nullptr) && !valueGroup.empty()) {
				auto info = SymbolInfo {};
				if(node.symbol() != nullptr) {
					auto tmpInfo = context.script().getSymbolInfo(*node.symbol());
					if(tmpInfo != nullptr) {
						info = *tmpInfo;
					}
				}
				valueGroup.push({ Instruction { Command::MOV, context.script().querySymbolOrValue(dest), valueGroup.value() }, SymbolInfo{ context.scope(), dest.name(), info.references } });
			}
			return valueGroup;
		}
	}
}

ska::bytecode::GenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::VARIABLE_AFFECTATION>::generate(OperateOn node, GenerationContext& context) {
	return CommonGenerate(*this, node.GetVariableNameNode(), node.GetVariableValueNode(), context);
}

ska::bytecode::GenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::VARIABLE_DECLARATION>::generate(OperateOn node, GenerationContext& context) {
	return CommonGenerate(*this, node.GetVariableNameNode(), node.GetVariableValueNode(), context);
}
