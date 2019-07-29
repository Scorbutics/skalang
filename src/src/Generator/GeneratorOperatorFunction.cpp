#include <string>
#include "GeneratorOperatorFunction.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScript.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::bytecode::GeneratorOperator<ska::Operator::FUNCTION_DECLARATION>);

#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::GeneratorOperator<ska::Operator::FUNCTION_DECLARATION>)

namespace ska {
	namespace bytecode {
		static GenerationOutput CommonGenerate(Generator& generator, const ASTNode& dest, const ska::ASTNode& node, GenerationContext& context) {
			auto valueGroup = generator.generate({ context.script(), node });
			valueGroup.push(InstructionPack{ Instruction { Command::MOV, context.script().queryVariableOrValue(dest) , context.script().queryVariableOrValue(node) } });
			return valueGroup;
		}

		static GenerationOutput AddRelativeJumpInstruction(GenerationOutput output) {
			auto ss = std::stringstream {};
			ss << output.pack().size();
			auto jumpInstruction = Instruction { Command::JUMP, Value {ss.str(), Type::MakeBuiltIn<ExpressionType::INT>()}};
			auto result = GenerationOutput{ std::move(jumpInstruction) };
			result.push(std::move(output));
			return result;
		}

		template <class NodeIterable>
		static void AccessParameters(Script& script, const NodeIterable& node, std::size_t parameterSize, Command command, GenerationOutput& output) {
			auto temporaryContainer = std::vector<Value> {};

			std::size_t index = 0u;
			for (const auto& paramNode : node) {
				if (paramNode != nullptr && index < parameterSize) {
					temporaryContainer.push_back(script.queryVariableOrValue(*paramNode));
					if (temporaryContainer.size() == 3) {
						output.push(Instruction { command, std::move(temporaryContainer)});
						temporaryContainer = {};
					}
				}
				index++;
			}
			assert(temporaryContainer.size() <= 3);
			if (!temporaryContainer.empty()) {
				output.push(Instruction { command, std::move(temporaryContainer) });
			}

			LOG_DEBUG << output.pack();
		}

	}
}

ska::bytecode::GenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::FUNCTION_DECLARATION>::generate(OperateOn node, GenerationContext& context) {
	LOG_DEBUG << "Generating prototype : ";
	auto valueGroup = m_generator.generate({ context.script(), node.GetFunctionPrototype() });
	LOG_DEBUG << "Generating body : ";
	valueGroup.push(m_generator.generate({ context.script(), node.GetFunctionBody() }));
	valueGroup.push(Instruction{ Command::END, Value{}});
	LOG_DEBUG << "\tPrototype and Body : " << valueGroup.pack();
	return AddRelativeJumpInstruction(std::move(valueGroup));
}

ska::bytecode::GenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::FUNCTION_PROTOTYPE_DECLARATION>::generate(OperateOn node, GenerationContext& context) {
	auto result = GenerationOutput { Instruction { Command::LABEL, Value {node.GetFunctionName(), node.GetFunction().type().value()}}};
	LOG_DEBUG << "\tLabel : " << result.pack();
	AccessParameters(context.script(), node, node.GetParameterSize(), Command::POP, result);
	LOG_DEBUG << "\tLabel and Parameters : " << result.pack();
	return result;
}

ska::bytecode::GenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::FUNCTION_CALL>::generate(OperateOn node, GenerationContext& context) {
	auto functionType = node.GetFunctionType();
	auto jumpInstruction = Instruction { Command::JUMP, context.script().queryVariableOrValue(node.GetFunctionNameNode(), &functionType) };
	auto result = GenerationOutput{ InstructionPack {} };

	AccessParameters(context.script(), node, node.GetFunctionParameterSize(), Command::PUSH, result);
	result.push(std::move(jumpInstruction));

	return result;
}
