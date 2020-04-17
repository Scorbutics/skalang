#include "Config/LoggerConfigLang.h"
#include <string>
#include "GeneratorOperatorFunction.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScriptGenerationHelper.h"
#include "Generator/ComputingOperations/BytecodeNLengthOperations.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::GeneratorOperator<ska::Operator::FUNCTION_DECLARATION>);

#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::GeneratorOperator<ska::Operator::FUNCTION_DECLARATION>) << "%01c"

namespace ska {
	namespace bytecode {
		static InstructionOutput AddRelativeJumpInstruction(InstructionOutput output) {
			auto jumpInstruction = Instruction { Command::JUMP_REL, Operand { static_cast<long>(output.size()), OperandType::PURE }};
			auto result = InstructionOutput{ std::move(jumpInstruction) };
			result.push(std::move(output));
			return result;
		}

	}
}

ska::bytecode::InstructionOutput ska::bytecode::GeneratorOperator<ska::Operator::FUNCTION_DECLARATION>::generate(OperateOn node, GenerationContext& context) {
	LOG_DEBUG << "Generating prototype of \"" << node.GetFunctionName() << "\"...";
	auto valueGroup = generateNext({ context, node.GetFunctionPrototype() });

	LOG_DEBUG << "Generating body...";
	const auto* functionSymbolInfo = context.getSymbolInfo(node.GetFunction());
	LOG_DEBUG << "Function Call symbol info : " << (functionSymbolInfo == nullptr || functionSymbolInfo->binding == std::numeric_limits<std::size_t>::max() ? "none" : "with binding");
	if (functionSymbolInfo == nullptr || functionSymbolInfo->binding == std::numeric_limits<std::size_t>::max() || functionSymbolInfo->bindingPassThrough) {
		valueGroup.push(generateNext({ context, node.GetFunctionBody(), 1 }));
	}

	LOG_DEBUG << "\nGenerated " << valueGroup << " with value " << valueGroup.operand();

	const auto returningFunctionType = node.GetFunctionPrototype().type().value().back();

	const auto isVoidReturningFunction = returningFunctionType == ExpressionType::VOID;
	valueGroup.push(Instruction{ Command::RET, isVoidReturningFunction ? Operand{} : valueGroup.operand() });

	auto fullFunction = AddRelativeJumpInstruction(std::move(valueGroup));
	fullFunction.push(Instruction{
		Command::END,
		context.querySymbolOrOperand(node.GetFunction()),
		Operand { -static_cast<long>(fullFunction.size()), OperandType::PURE }});

	return fullFunction;
}

ska::bytecode::InstructionOutput ska::bytecode::GeneratorOperator<ska::Operator::FUNCTION_PROTOTYPE_DECLARATION>::generate(OperateOn node, GenerationContext& context) {
	auto result = InstructionOutput{ };
	applyGenerator(ApplyNOperations<Command::POP, OperateOn&>, result, context, node, node.GetParameterSize());
	LOG_DEBUG << "\tParameters : " << result;
	return result;
}

ska::bytecode::InstructionOutput ska::bytecode::GeneratorOperator<ska::Operator::FUNCTION_CALL>::generate(OperateOn node, GenerationContext& context) {
	auto preCallValue = generateNext({context, node.GetFunctionNameNode()});
	LOG_DEBUG << "Function call : "<< node.GetFunctionNameNode().name() << " of type " << node.GetFunctionType();

	// Handle built-in type functions : no symbol provided
	const auto* functionTypeSymbol = node.GetFunctionNameNode().typeSymbol();
	if (functionTypeSymbol == nullptr) {
		return preCallValue;
	}

	// Handle custom type functions
	const auto* functionSymbolInfo = context.getSymbolInfo(*functionTypeSymbol);
	LOG_DEBUG << "Function Call symbol info : " << (functionSymbolInfo == nullptr || functionSymbolInfo->binding == std::numeric_limits<std::size_t>::max() ? "none" : "with binding");

	auto callInstruction = InstructionOutput {};
	if (functionSymbolInfo != nullptr && functionSymbolInfo->binding != std::numeric_limits<std::size_t>::max()) {
		callInstruction.push(Instruction{ Command::BIND, Operand { ScriptVariableRef{ functionSymbolInfo->binding, functionSymbolInfo->script}, OperandType::BIND_NATIVE}, Operand {static_cast<long>(node.GetFunctionParameterSize()), OperandType::PURE} });
		if (functionSymbolInfo->bindingPassThrough) {
			callInstruction.push(Instruction{ Command::JUMP_ABS, std::move(preCallValue.operand()) });
		}
	} else {
		callInstruction.push(Instruction { Command::JUMP_ABS, std::move(preCallValue.operand()) });
	}
	auto result = std::move(preCallValue);

	applyGenerator(ApplyNOperations<Command::PUSH, OperateOn&>, result, context, node, node.GetFunctionParameterSize());
	LOG_DEBUG << " PUSH result : " << result;

	result.push(std::move(callInstruction));
	if(node.GetFunctionType().back() != ExpressionType::VOID) {
		result.push(Instruction{ Command::POP, context.queryNextRegister()});
	}

	LOG_DEBUG << "Output : " << result;
	return result;
}
