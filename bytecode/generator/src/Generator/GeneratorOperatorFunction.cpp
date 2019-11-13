#include "Config/LoggerConfigLang.h"
#include <string>
#include "GeneratorOperatorFunction.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScriptGenerationService.h"
#include "Generator/ComputingOperations/BytecodeNLengthOperations.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::GeneratorOperator<ska::Operator::FUNCTION_DECLARATION>);

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
		static ScriptGenerationOutput AddRelativeJumpInstruction(ScriptGenerationOutput output) {
			auto jumpInstruction = Instruction { Command::JUMP_REL, Value { static_cast<long>(output.size()) }};
			auto result = ScriptGenerationOutput{ std::move(jumpInstruction) };
			result.push(std::move(output));
			return result;
		}

	}
}

ska::bytecode::ScriptGenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::FUNCTION_DECLARATION>::generate(OperateOn node, GenerationContext& context) {
	LOG_DEBUG << "Generating prototype of \"" << node.GetFunctionName() << "\"...";
	auto valueGroup = generateNext({ context, node.GetFunctionPrototype() });
	LOG_DEBUG << "Generating body...";
	valueGroup.push(generateNext({ context, node.GetFunctionBody(), 1 }));

	LOG_DEBUG << "\nGenerated " << valueGroup << " with value " << valueGroup.value();

	const auto returningFunctionType = node.GetFunctionPrototype().type().value().compound().back();

	const auto isVoidReturningFunction = returningFunctionType == ExpressionType::VOID;
	valueGroup.push(Instruction{ Command::RET, isVoidReturningFunction ? Value{} : valueGroup.value() });

	auto fullFunction = AddRelativeJumpInstruction(std::move(valueGroup));
	fullFunction.push(Instruction{
		Command::END,
		context.querySymbolOrValue(node.GetFunction()),
		Value { -static_cast<long>(fullFunction.size()) }});

	return fullFunction;
}

ska::bytecode::ScriptGenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::FUNCTION_PROTOTYPE_DECLARATION>::generate(OperateOn node, GenerationContext& context) {
	auto result = ScriptGenerationOutput{ };
	ApplyNOperations<Command::POP>(result, context, node, node.GetParameterSize());
	LOG_DEBUG << "\tParameters : " << result;
	return result;
}

ska::bytecode::ScriptGenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::FUNCTION_CALL>::generate(OperateOn node, GenerationContext& context) {
	auto preCallValue = generateNext({context, node.GetFunctionNameNode()});
	LOG_DEBUG << "Function call : "<< node.GetFunctionNameNode().name() << " of type " << node.GetFunctionType();

	auto callInstruction = Instruction { Command::JUMP_ABS, std::move(preCallValue.value()) };
	auto result = std::move(preCallValue);

	ApplyNOperations<Command::PUSH>(result, context, node, node.GetFunctionParameterSize());
	LOG_DEBUG << " PUSH result : " << result;

	result.push(std::move(callInstruction));
	if(node.GetFunctionType().compound().back() != ExpressionType::VOID) {
		result.push(Instruction{ Command::POP, context.script().queryNextRegister()});
	}

	LOG_DEBUG << "Output : " << result;
	return result;
}