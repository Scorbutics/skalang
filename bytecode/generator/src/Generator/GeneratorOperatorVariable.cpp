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
				/*LOG_DEBUG << "Storage instruction for type " << node.type().value() << " is " << node.type().value().symbolType();
				const auto type = valueGroup.value().toString();
				if(!type.empty() && type[0] == 'L') {
					LOG_DEBUG << "Label";
					valueGroup.push(InstructionPack{ Instruction { Command::LABEL_AS_REF, context.script().queryVariableOrValue(dest), valueGroup.value() } });
				} else {
					LOG_DEBUG << "Variable or register";*/
				valueGroup.push({ Instruction { Command::MOV, context.script().querySymbolOrValue(dest), valueGroup.value() }, SymbolInfo{ context.scope(), dest.name() } });
				/*}*/
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
