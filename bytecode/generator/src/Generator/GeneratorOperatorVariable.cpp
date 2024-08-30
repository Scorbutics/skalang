#include "Config/LoggerConfigLang.h"
#include <string>
#include "GeneratorOperatorVariable.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScriptGenerationHelper.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::GeneratorOperator<ska::Operator::DECLARATION>);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::GeneratorOperator<ska::Operator::DECLARATION>)

namespace ska {
	namespace bytecode {
		template <class Generator>
		static InstructionOutput CommonGenerate(Generator& generator, const ASTNode& dest, const ska::ASTNode& node, GenerationContext& context, bool capture) {
			auto finalGroup = generator.generateNext(context.next(node));
			auto operandDestination = finalGroup.operand();
			if((dest.symbol() != node.symbol() || node.symbol() == nullptr) && !finalGroup.empty()) {
				LOG_DEBUG << "Creating MOV instruction with operand group " << finalGroup;
				// TODO: if we use a variable of outer scope inside a function, create some specific instructions here in order to capture the env.
				auto variable = dest.isSymbolicLeaf() ? InstructionOutput{context.querySymbolOrOperand(dest, capture)} : generator.generateNext(context.next(dest));
				auto variableDestination = variable.operand();

				finalGroup.push(std::move(variable));
				finalGroup.push(Instruction { Command::MOV, variableDestination, operandDestination });
				LOG_DEBUG << "\tin operand " << operandDestination;
			}
			return finalGroup;
		}
	}
}

ska::bytecode::InstructionOutput ska::bytecode::GeneratorOperator<ska::Operator::AFFECTATION>::generate(OperateOn node, GenerationContext& context) {
	return CommonGenerate(*this, node.GetVariableNameNode(), node.GetVariableValueNode(), context, true);
}

ska::bytecode::InstructionOutput ska::bytecode::GeneratorOperator<ska::Operator::DECLARATION>::generate(OperateOn node, GenerationContext& context) {
	// Only try capture if the variable is already existing
	const auto capture = node.GetVariableNameNode().symbol() != nullptr && context.getSymbol(*node.GetVariableNameNode().symbol()).has_value();
	return CommonGenerate(*this, node.GetVariableNameNode(), node.GetVariableValueNode(), context, capture);
}

ska::bytecode::InstructionOutput ska::bytecode::GeneratorOperator<ska::Operator::PARAMETER_DECLARATION>::generate(OperateOn node, GenerationContext& context) {
	return { context.querySymbolOrOperand(node.GetVariableNameNode()) };
}
