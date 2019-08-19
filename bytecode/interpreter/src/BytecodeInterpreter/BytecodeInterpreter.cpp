#include "BytecodeInterpreter.h"

#include "NodeValue/AST.h"
#include "Generator/Value/BytecodeScript.h"

#include "Units/InterpreterCommandMov.h"
#include "Units/InterpreterCommandAddI.h"
#include "Units/InterpreterCommandSubI.h"
#include "Units/InterpreterCommandMulI.h"
#include "Units/InterpreterCommandDivI.h"

#include "InterpreterDeclarer.h"

ska::bytecode::Interpreter::CommandInterpreter ska::bytecode::Interpreter::build() {
	auto result = CommandInterpreter {};
	static constexpr auto maxCommandEnumIndex = static_cast<std::size_t>(Command::UNUSED_Last_Length);
	result.resize(maxCommandEnumIndex);

	InterpreterCommandDeclare<Command::MOV>(*this, result);

	InterpreterCommandDeclare<Command::ADD_I>(*this, result);
	InterpreterCommandDeclare<Command::SUB_I>(*this, result);
	InterpreterCommandDeclare<Command::MUL_I>(*this, result);
	InterpreterCommandDeclare<Command::DIV_I>(*this, result);

	return result;
}

ska::bytecode::Interpreter::Interpreter(const ReservedKeywordsPool& reserved) :
	m_commandInterpreter(build()) {
}

ska::bytecode::ExecutionOutput ska::bytecode::Interpreter::interpret(ExecutionContext& node) {
	auto& builder = m_commandInterpreter[static_cast<std::size_t>(node.currentInstruction().command())];
	assert(builder != nullptr);

	for(auto continueExecution = !node.empty(); continueExecution; continueExecution = node.incInstruction()) {
		auto result = builder->interpret(node);
	}

	return {};
}
