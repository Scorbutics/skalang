#include "Config/LoggerConfigLang.h"
#include "BytecodeInterpreter.h"

#include "NodeValue/AST.h"
#include "BytecodeScript.h"
#include "Generator/Value/BytecodeGenerationOutput.h"

#include "Units/InterpreterCommandMov.h"
#include "Units/InterpreterCommandAddI.h"
#include "Units/InterpreterCommandSubI.h"
#include "Units/InterpreterCommandMulI.h"
#include "Units/InterpreterCommandDivI.h"
#include "Units/InterpreterCommandAddD.h"
#include "Units/InterpreterCommandSubD.h"
#include "Units/InterpreterCommandMulD.h"
#include "Units/InterpreterCommandDivD.h"
#include "Units/InterpreterCommandAddStr.h"
#include "Units/InterpreterCommandPushArrArr.h"
#include "Units/InterpreterCommandSubArr.h"
#include "Units/InterpreterCommandCmpStr.h"
#include "Units/InterpreterCommandCmpArr.h"
#include "Units/InterpreterCommandTestEq.h"
#include "Units/InterpreterCommandTestG.h"
#include "Units/InterpreterCommandTestGe.h"
#include "Units/InterpreterCommandTestL.h"
#include "Units/InterpreterCommandTestLe.h"
#include "Units/InterpreterCommandTestNeq.h"
#include "Units/InterpreterCommandPushFArr.h"
#include "Units/InterpreterCommandPushBArr.h"
#include "Units/InterpreterCommandRet.h"
#include "Units/InterpreterCommandEnd.h"
#include "Units/InterpreterCommandPush.h"
#include "Units/InterpreterCommandPop.h"
#include "Units/InterpreterCommandPopInArr.h"
#include "Units/InterpreterCommandPopInVar.h"
#include "Units/InterpreterCommandJumpAbs.h"
#include "Units/InterpreterCommandJumpRel.h"
#include "Units/InterpreterCommandConvID.h"
#include "Units/InterpreterCommandConvDI.h"
#include "Units/InterpreterCommandConvDStr.h"
#include "Units/InterpreterCommandConvIStr.h"
#include "Units/InterpreterCommandArrAccess.h"
#include "Units/InterpreterCommandScript.h"
#include "Units/InterpreterCommandJumpNif.h"
#include "InterpreterDeclarer.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::Interpreter);

#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::Interpreter)
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::bytecode::Interpreter)

ska::bytecode::Interpreter::CommandInterpreter ska::bytecode::Interpreter::build(Generator& generator) {
	auto result = CommandInterpreter {};
	static constexpr auto maxCommandEnumIndex = static_cast<std::size_t>(Command::UNUSED_Last_Length);
	result.resize(maxCommandEnumIndex);

	InterpreterCommandDeclare<Command::MOV>(*this, result, generator);

	InterpreterCommandDeclare<Command::ADD_I>(*this, result, generator);
	InterpreterCommandDeclare<Command::SUB_I>(*this, result, generator);
	InterpreterCommandDeclare<Command::MUL_I>(*this, result, generator);
	InterpreterCommandDeclare<Command::DIV_I>(*this, result, generator);

	InterpreterCommandDeclare<Command::ADD_D>(*this, result, generator);
	InterpreterCommandDeclare<Command::SUB_D>(*this, result, generator);
	InterpreterCommandDeclare<Command::MUL_D>(*this, result, generator);
	InterpreterCommandDeclare<Command::DIV_D>(*this, result, generator);

	InterpreterCommandDeclare<Command::ADD_STR>(*this, result, generator);
	InterpreterCommandDeclare<Command::PUSH_ARR_ARR>(*this, result, generator);
	InterpreterCommandDeclare<Command::PUSH_F_ARR>(*this, result, generator);
	InterpreterCommandDeclare<Command::PUSH_B_ARR>(*this, result, generator);
	InterpreterCommandDeclare<Command::SUB_ARR>(*this, result, generator);

	InterpreterCommandDeclare<Command::CONV_D_I>(*this, result, generator);
	InterpreterCommandDeclare<Command::CONV_I_D>(*this, result, generator);
	InterpreterCommandDeclare<Command::CONV_I_STR>(*this, result, generator);
	InterpreterCommandDeclare<Command::CONV_D_STR>(*this, result, generator);

	InterpreterCommandDeclare<Command::CMP_STR>(*this, result, generator);
	InterpreterCommandDeclare<Command::CMP_ARR>(*this, result, generator);
	InterpreterCommandDeclare<Command::TEST_EQ>(*this, result, generator);
	InterpreterCommandDeclare<Command::TEST_G>(*this, result, generator);
	InterpreterCommandDeclare<Command::TEST_GE>(*this, result, generator);
	InterpreterCommandDeclare<Command::TEST_L>(*this, result, generator);
	InterpreterCommandDeclare<Command::TEST_LE>(*this, result, generator);
	InterpreterCommandDeclare<Command::TEST_NEQ>(*this, result, generator);

	InterpreterCommandDeclare<Command::RET>(*this, result, generator);
	InterpreterCommandDeclare<Command::END>(*this, result, generator);
	InterpreterCommandDeclare<Command::PUSH>(*this, result, generator);
	InterpreterCommandDeclare<Command::POP>(*this, result, generator);
	InterpreterCommandDeclare<Command::POP_IN_ARR>(*this, result, generator);
	InterpreterCommandDeclare<Command::POP_IN_VAR>(*this, result, generator);
	InterpreterCommandDeclare<Command::JUMP_ABS>(*this, result, generator);
	InterpreterCommandDeclare<Command::JUMP_REL>(*this, result, generator);
	InterpreterCommandDeclare<Command::JUMP_NIF>(*this, result, generator);

	InterpreterCommandDeclare<Command::ARR_ACCESS>(*this, result, generator);
	InterpreterCommandDeclare<Command::SCRIPT>(*this, result, generator);

	return result;
}

ska::bytecode::Interpreter::Interpreter(Generator& generator, const ReservedKeywordsPool& reserved) :
	m_commandInterpreter(build(generator)) {
}

void ska::bytecode::Interpreter::interpret(ExecutionContext& node) {
	for(auto continueExecution = !node.idle(); continueExecution; continueExecution = node.incInstruction()) {
		auto& instruction = node.currentInstruction();
		LOG_INFO << "Interpreting " << instruction;
		auto& builder = m_commandInterpreter[static_cast<std::size_t>(instruction.command())];
		assert(builder != nullptr);
		auto nodeValue = builder->interpret(node);
		if (!nodeValue.empty()) {
			LOG_DEBUG << "Setting " << nodeValue.convertString() << " for " << instruction.dest();
			node.set(instruction.dest(), std::move(nodeValue));
		}
	}
}

void ska::bytecode::Interpreter::interpret(InterpretationPart target) {
	/*
	auto output = std::make_unique<ska::bytecode::Executor>();
	auto context = ExecutionContext {*output, ..., ... };
	*/
}

std::unique_ptr<ska::bytecode::Executor> ska::bytecode::Interpreter::interpret(std::size_t scriptIndex, GenerationOutput& instructions) {
	auto output = std::make_unique<ska::bytecode::Executor>();
	auto context = ExecutionContext {*output, scriptIndex, instructions };
	interpret(context);
	return output;
}
