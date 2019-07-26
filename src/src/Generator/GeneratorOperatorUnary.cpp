#include <string>
#include "NodeValue/AST.h"
#include "GeneratorOperatorUnary.h"
#include "Interpreter/Value/Script.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScript.h"

namespace ska {
	namespace bytecode {
		static GenerationOutput CommonGenerate(Generator& generator, GenerationContext& context) {
			/*if (generator.reg().hasCommand()) {
				return {};
			}*/
			auto result = Value {context.pointer().name(), context.pointer().type().value() };
			//auto result = context.cellFromValue(ska::BytecodeCommand::OUT);
			//context.script().setReg(result);
			return { std::move(result) };
		}
	}
}

ska::bytecode::GenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::UNARY>::generate(OperateOn node, GenerationContext& context) {
	return CommonGenerate(m_generator, context);
}

ska::bytecode::GenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::LITERAL>::generate(OperateOn node, GenerationContext& context) {
	return CommonGenerate(m_generator, context);
}
