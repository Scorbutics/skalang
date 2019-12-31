#include "Config/LoggerConfigLang.h"
#include <string>
#include "GeneratorOperatorFunction.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScriptGenerationHelper.h"
#include "Generator/ComputingOperations/BytecodeNLengthOperations.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::bytecode::GeneratorOperator<ska::Operator::FUNCTION_DECLARATION>);

#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::GeneratorOperator<ska::Operator::FUNCTION_DECLARATION>) << "%01c"

/*

var toto = function() : var {
	var priv_test = 1;
	return {
		test : priv_test,
		say : function(more : string) : string {
			var s = "lol" + priv_test + more;
			return s;
		}
	};
};

var test = toto();
___________________________________________________________________________________

JUMP...
LABEL toto //label 0
	MOV V0 1

	PUSH V0

JUMP 10
LABEL toto say //label 1
	POP V1
	CONV_I_STR R0 V0
	ADD_STR R1 "lol" R0
	ADD_STR R2 R1 V1
	MOV V2 R2
	END V2

	LABEL_REF V3, toto say
	PUSH V3

	END_STACK 2

CALL toto
POP_IN_VAR V3, 2

*/

namespace ska {
	namespace bytecode {
		static InstructionOutput AddRelativeJumpInstruction(InstructionOutput output) {
			auto jumpInstruction = Instruction { Command::JUMP_REL, Operand { static_cast<long>(output.size()) }};
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
	LOG_DEBUG << "Function Call symbol info : " << (functionSymbolInfo == nullptr || functionSymbolInfo->binding == nullptr ? "none" : "with binding");
	if (functionSymbolInfo == nullptr || functionSymbolInfo->binding == nullptr) {
		valueGroup.push(generateNext({ context, node.GetFunctionBody(), 1 }));
	} else {
		//valueGroup.push(Instruction {});
	}

	LOG_DEBUG << "\nGenerated " << valueGroup << " with value " << valueGroup.operand();

	const auto returningFunctionType = node.GetFunctionPrototype().type().value().compound().back();

	const auto isVoidReturningFunction = returningFunctionType == ExpressionType::VOID;
	valueGroup.push(Instruction{ Command::RET, isVoidReturningFunction ? Operand{} : valueGroup.operand() });

	auto fullFunction = AddRelativeJumpInstruction(std::move(valueGroup));
	fullFunction.push(Instruction{
		Command::END,
		context.querySymbolOrOperand(node.GetFunction()),
		Operand { -static_cast<long>(fullFunction.size()) }});

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

	const auto* functionSymbolInfo = context.getSymbolInfo(*node.GetFunctionNameNode().type().value().symbol());
	LOG_DEBUG << "Function Call symbol info : " << (functionSymbolInfo == nullptr || functionSymbolInfo->binding == nullptr ? "none" : "with binding");

	auto callInstruction = Instruction {};
	if (functionSymbolInfo != nullptr && functionSymbolInfo->binding != nullptr) {
		callInstruction = Instruction{ Command::BIND, context.storeBinding(functionSymbolInfo->binding), Operand {static_cast<long>(node.GetFunctionParameterSize())} };
	} else {
		callInstruction = Instruction { Command::JUMP_ABS, std::move(preCallValue.operand()) };
	}
	auto result = std::move(preCallValue);

	applyGenerator(ApplyNOperations<Command::PUSH, OperateOn&>, result, context, node, node.GetFunctionParameterSize());
	LOG_DEBUG << " PUSH result : " << result;

	result.push(std::move(callInstruction));
	if(node.GetFunctionType().compound().back() != ExpressionType::VOID) {
		result.push(Instruction{ Command::POP, context.queryNextRegister()});
	}

	LOG_DEBUG << "Output : " << result;
	return result;
}
