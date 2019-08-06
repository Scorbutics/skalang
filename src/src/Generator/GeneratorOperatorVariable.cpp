#include <string>
#include "GeneratorOperatorVariable.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScript.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::GeneratorOperator<ska::Operator::VARIABLE_DECLARATION>);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::GeneratorOperator<ska::Operator::VARIABLE_DECLARATION>)

namespace ska {
	namespace bytecode {
		static GenerationOutput CommonGenerate(Generator& generator, const ASTNode& dest, const ska::ASTNode& node, GenerationContext& context) {
			auto valueGroup = generator.generate({ context.script(), node });
			if((dest.symbol() != node.symbol() || node.symbol() == nullptr) && !valueGroup.empty()) {
				/*LOG_DEBUG << "Storage instruction for type " << node.type().value();
				const auto type = valueGroup.value().content;
				if(!type.empty() && type[0] == 'L') {
					LOG_DEBUG << "Label";
					valueGroup.push(InstructionPack{ Instruction { Command::LABEL_AS_REF, context.script().queryVariableOrValue(dest), valueGroup.value() } });
				} else {
					LOG_DEBUG << "Variable or register";*/
					valueGroup.push(InstructionPack{ Instruction { Command::MOV, context.script().queryVariableOrValue(dest), valueGroup.value() } });
				/*}*/
			}
			return valueGroup;
		}
	}
}

ska::bytecode::GenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::VARIABLE_AFFECTATION>::generate(OperateOn node, GenerationContext& context) {
	return CommonGenerate(m_generator, node.GetVariableNameNode(), node.GetVariableValueNode(), context);
}

ska::bytecode::GenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::VARIABLE_DECLARATION>::generate(OperateOn node, GenerationContext& context) {
	return CommonGenerate(m_generator, node.GetVariableNameNode(), node.GetVariableValueNode(), context);
}
