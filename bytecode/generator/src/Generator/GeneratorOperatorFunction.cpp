#include <string>
#include "GeneratorOperatorFunction.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScript.h"
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
		static GenerationOutput AddRelativeJumpInstruction(GenerationOutput output) {
			auto jumpInstruction = Instruction { Command::JUMP_REL, Value { static_cast<long>(output.size()) }};
			auto result = GenerationOutput{ std::move(jumpInstruction) };
			result.push(std::move(output));
			return result;
		}

	}
}

ska::bytecode::GenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::FUNCTION_DECLARATION>::generate(OperateOn node, GenerationContext& context) {
	LOG_DEBUG << "Generating prototype of \"" << node.GetFunctionName() << "\"...";
	auto valueGroup = generateNext({ context.script(), node.GetFunctionPrototype(), context.scope() });
	LOG_DEBUG << "Generating body...";
	valueGroup.push(generateNext({ context.script(), node.GetFunctionBody(), context.scope() + 1 }));

	LOG_DEBUG << "\nGenerated " << valueGroup << " with value " << valueGroup.value();

	const auto returningFunctionType = node.GetFunctionPrototype().type().value().compound().back();

	auto& bodyNode = node.GetFunctionBody();
	if(bodyNode.size() > 0) {
		auto& returnNode = bodyNode[bodyNode.size() - 1];
		if(returnNode.size() > 0) {
			if(returningFunctionType == ExpressionType::OBJECT) {
				assert(returnNode[0].size() > 0);

				auto& firstField = returnNode[0][0];
				const auto* infos = context.script().getSymbolInfo(firstField);
				if(infos != nullptr) {
					LOG_DEBUG << "Symbol info detected for current function " << *infos << " setted as " << node.GetFunction();
					context.script().setSymbolInfo(node.GetFunction(), *infos);
				} else {
					LOG_DEBUG << "No Symbol info for node \"" << firstField.name() << "\"";
				}
			} else {
				LOG_DEBUG << "Returned symbol is \"" << returnNode[0].name() << "\"";
			}
		}
	}

	const auto isVoidReturningFunction = returningFunctionType == ExpressionType::VOID;
	valueGroup.push(Instruction{ Command::RET, isVoidReturningFunction ? Value{} : valueGroup.value() });

	auto fullFunction = AddRelativeJumpInstruction(std::move(valueGroup));
	fullFunction.push(Instruction{
		Command::END,
		context.script().querySymbolOrValue(node.GetFunction()),
		Value { -static_cast<long>(fullFunction.size()) }});

	return fullFunction;
}

ska::bytecode::GenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::FUNCTION_PROTOTYPE_DECLARATION>::generate(OperateOn node, GenerationContext& context) {
	auto result = GenerationOutput{ };
	ApplyNOperations<Command::POP>(result, context.script(), node, node.GetParameterSize());
	LOG_DEBUG << "\tParameters : " << result;
	return result;
}

ska::bytecode::GenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::FUNCTION_CALL>::generate(OperateOn node, GenerationContext& context) {
	auto preCallValue = generateNext({context.script(), node.GetFunctionNameNode(), context.scope()});
	LOG_DEBUG << "Function call : "<< node.GetFunctionNameNode().type().value();
	const auto* functionReferencedSymbol = node.GetFunctionNameNode().type().value().symbol();
	assert(functionReferencedSymbol != nullptr);
	auto symbolValue = context.script().querySymbol(*functionReferencedSymbol);
	LOG_DEBUG << " Call referenced as symbol : "<< symbolValue;
	auto callInstruction = Instruction { Command::JUMP_ABS, std::move(symbolValue) };
	auto result = std::move(preCallValue);

	ApplyNOperations<Command::PUSH>(result, context.script(), node, node.GetFunctionParameterSize());
	LOG_DEBUG << result;

	result.push(std::move(callInstruction));
	result.push(Instruction{ Command::POP, context.script().queryNextRegister()});
	return result;
}
