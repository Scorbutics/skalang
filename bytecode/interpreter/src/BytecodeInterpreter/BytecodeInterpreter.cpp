#include "Config/LoggerConfigLang.h"
#include "BytecodeInterpreter.h"

#include "NodeValue/AST.h"
#include "Generator/Value/BytecodeScript.h"

#include "Units/InterpreterCommandMov.h"
#include "Units/InterpreterCommandAddI.h"
#include "Units/InterpreterCommandSubI.h"
#include "Units/InterpreterCommandMulI.h"
#include "Units/InterpreterCommandDivI.h"

#include "InterpreterDeclarer.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::bytecode::Interpreter);

#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::Interpreter)

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
	auto lastValue = Value{};
	for(auto continueExecution = !node.empty(); continueExecution; continueExecution = node.incInstruction()) {
		auto& instruction = node.currentInstruction();
		auto& builder = m_commandInterpreter[static_cast<std::size_t>(instruction.command())];
		assert(builder != nullptr);
		auto result = builder->interpret(node);
		node.set(instruction.dest(), std::move(result));
	}

	return lastValue.empty() ? ExecutionOutput{} : std::move(node.getVariant(lastValue));
}

ska::bytecode::ExecutionOutput ska::bytecode::Interpreter::interpret(GenerationOutput& instructions) {
	LOG_DEBUG << "Interpreting " << instructions;

	auto context = ExecutionContext { instructions };
	return interpret(context);
}
