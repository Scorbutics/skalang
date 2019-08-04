#include <string>
#include "NodeValue/AST.h"
#include "GeneratorOperatorFor.h"
#include "Interpreter/Value/Script.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScript.h"

ska::bytecode::GenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::FOR_LOOP>::generate(OperateOn node, GenerationContext& context) {
	/*
	Example :
		// Initialization part
		{ Command::MOV, "V0", "0" },

		// Condition part
		{ Command::SUB_I, "R0", "V0", "10" },
		{ Command::TEST_L, "R0", "R0" },
		{ Command::JUMP_NIF, "R0", "5" },

		// Body part
		{ Command::MOV, "V1", "123" },
		{ Command::ADD_I, "R1", "V1", "V0" },

		// Increment part
		{ Command::ADD_I, "R2", "V0", "1"},
		{ Command::MOV, "V0", "R2" },
		{ Command::JUMP, "-8" }
	*/
	
	auto initGroup = m_generator.generate({ context.script(), node.GetInitialization() });

	auto conditionGroup = m_generator.generate({ context.script(), node.GetCondition() });
	auto bodyGroup = m_generator.generate({ context.script(), node.GetStatement() });
	auto incrementGroup = m_generator.generate({ context.script(), node.GetIncrement() });

	if (!incrementGroup.value().empty()) {
		conditionGroup.push(Instruction{ Command::JUMP_NIF, conditionGroup.value(), Value { std::to_string(bodyGroup.pack().size() + incrementGroup.pack().size() + 1) } });
	}
	incrementGroup.push(Instruction{ Command::JUMP, Value { std::to_string( - static_cast<long>(conditionGroup.pack().size() + bodyGroup.pack().size() + incrementGroup.pack().size() + 1 )) } });

	initGroup.push(std::move(initGroup));
	initGroup.push(std::move(conditionGroup));
	initGroup.push(std::move(bodyGroup));
	initGroup.push(std::move(incrementGroup));
	return initGroup;
}
