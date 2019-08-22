#include <string>
#include "GeneratorOperatorFunction.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScript.h"
#include "Generator/ComputingOperations/BytecodeNLengthOperations.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::GeneratorOperator<ska::Operator::FUNCTION_DECLARATION>);

#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::GeneratorOperator<ska::Operator::FUNCTION_DECLARATION>)

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
		static GenerationOutput AddRelativeJumpInstruction(GenerationOutput output) {
			auto jumpInstruction = Instruction { Command::JUMP_REL, Value { static_cast<long>(output.size()) }};
			auto result = GenerationOutput{ std::move(jumpInstruction) };
			result.push(std::move(output));
			return result;
		}

	}
}

ska::bytecode::GenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::FUNCTION_DECLARATION>::generate(OperateOn node, GenerationContext& context) {
	LOG_DEBUG << "Generating prototype : ";
	auto valueGroup = m_generator.generate({ context.script(), node.GetFunctionPrototype() });
	LOG_DEBUG << "Generating body : ";
	valueGroup.push(m_generator.generate({ context.script(), node.GetFunctionBody() }));

	LOG_DEBUG << "Generated " << valueGroup << " with value " << valueGroup.value();

	const auto isVoidReturningFunction = node.GetFunctionPrototype().type().value().compound().back() == ExpressionType::VOID;
	valueGroup.push(Instruction{ Command::END, context.script().querySymbolOrValue(node.GetFunction()), valueGroup.name(), isVoidReturningFunction ? Value{} : valueGroup.value() });

	LOG_DEBUG << "\tPrototype and Body : " << valueGroup;
	auto fullFunction = AddRelativeJumpInstruction(std::move(valueGroup));
	return fullFunction;
}

ska::bytecode::GenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::FUNCTION_PROTOTYPE_DECLARATION>::generate(OperateOn node, GenerationContext& context) {
	auto result = GenerationOutput { Instruction { Command::LABEL, context.script().queryLabel(node.GetFunction())}};
	LOG_DEBUG << "\tLabel : " << result;
	ApplyNOperations(context.script(), node, Command::POP, result, node.GetParameterSize());
	LOG_DEBUG << "\tLabel and Parameters : " << result;
	return result;
}

ska::bytecode::GenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::FUNCTION_CALL>::generate(OperateOn node, GenerationContext& context) {
	auto callInstruction = Instruction { Command::JUMP_ABS, context.script().querySymbolOrValue(node.GetFunctionNameNode()) };
	auto result = GenerationOutput{ InstructionPack {} };

	ApplyNOperations(context.script(), node, Command::PUSH, result, node.GetFunctionParameterSize());
	LOG_DEBUG << result;
	result.push(std::move(callInstruction));
	result.push(Instruction{ Command::POP, context.script().queryNextRegister()});
	return result;
}
