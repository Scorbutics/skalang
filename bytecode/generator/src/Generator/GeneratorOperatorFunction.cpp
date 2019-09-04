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
	auto valueGroup = generateNext({ context.script(), node.GetFunctionPrototype(), context.scope() });
	LOG_DEBUG << "Generating body : ";
	valueGroup.push(generateNext({ context.script(), node.GetFunctionBody(), context.scope() + 1 }));

	LOG_DEBUG << "Generated " << valueGroup << " with value " << valueGroup.value();

	const auto returningFunctionType = node.GetFunctionPrototype().type().value().compound().back();
	const auto isVoidReturningFunction = returningFunctionType == ExpressionType::VOID;
	valueGroup.push({ Instruction{ Command::RET, isVoidReturningFunction ? Value{} : valueGroup.value() }, valueGroup.symbols().empty() ? SymbolInfo{} : valueGroup.symbols().back()});

	LOG_DEBUG << "\tPrototype and Body : " << valueGroup;
	auto fullFunction = AddRelativeJumpInstruction(std::move(valueGroup));
	fullFunction.push({ Instruction{
		Command::END,
		context.script().querySymbolOrValue(node.GetFunction()),
		Value { -static_cast<long>(fullFunction.size()) }},
		fullFunction.symbols().back()
	});

	if(returningFunctionType == ExpressionType::OBJECT) {
		// Associates field references (fullFunction.symbols().back()) to the pure unique symbol (*node.GetFunction().symbol())
		context.script().setSymbolInfo(node.GetFunction(), fullFunction.symbols().back());
	}
	return fullFunction;
}

ska::bytecode::GenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::FUNCTION_PROTOTYPE_DECLARATION>::generate(OperateOn node, GenerationContext& context) {
	auto result = GenerationOutput{ /*Instruction { Command::LABEL, context.script().queryLabel(node.GetFunction())} */ };
	//LOG_DEBUG << "\tLabel : " << result;
	ApplyNOperations<Command::POP>(result, context.script(), SymbolInfo{ context.scope(), node.GetFunction().name() }, node, node.GetParameterSize());
	LOG_DEBUG << "\tLabel and Parameters : " << result;
	return result;
}

ska::bytecode::GenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::FUNCTION_CALL>::generate(OperateOn node, GenerationContext& context) {
	//TODO support arbitrary function-expression call
	auto callInstruction = Instruction { Command::JUMP_ABS, context.script().querySymbolOrValue(node.GetFunctionNameNode()) };
	auto result = GenerationOutput{ };

	ApplyNOperations<Command::PUSH>(result, context.script(), {}, node, node.GetFunctionParameterSize());
	LOG_DEBUG << result;

	const auto* functionReturnValueSymbolInfo = context.script().getSymbolInfo(*node.GetFunctionNameNode().symbol());

	result.push({std::move(callInstruction), functionReturnValueSymbolInfo == nullptr ? SymbolInfo{} : *functionReturnValueSymbolInfo });
	result.push(Instruction{ Command::POP, context.script().queryNextRegister()});
	return result;
}
