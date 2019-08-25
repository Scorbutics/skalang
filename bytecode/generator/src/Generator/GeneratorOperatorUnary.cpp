#include <string>
#include "NodeValue/AST.h"
#include "GeneratorOperatorUnary.h"
#include "NodeValue/ScriptAST.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScript.h"

namespace ska {
	namespace bytecode {
		static GenerationOutput CommonGenerate(GenerationContext& context) {
			auto result = context.script().querySymbolOrValue(context.pointer());
			return { std::move(result) };
		}
	}
}

ska::bytecode::GenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::UNARY>::generate(OperateOn node, GenerationContext& context) {
	return CommonGenerate(context);
}

ska::bytecode::GenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::LITERAL>::generate(OperateOn node, GenerationContext& context) {
	return CommonGenerate(context);
}
