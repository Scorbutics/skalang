#include "Config/LoggerConfigLang.h"
#include "BytecodeInterpreter.h"

#include "NodeValue/AST.h"
#include "Generator/Value/BytecodeScript.h"

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
#include "InterpreterDeclarer.h"

SKA_LOGC_CONFIG(ska::LogLevel::Info, ska::bytecode::Interpreter);

#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::Interpreter)
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::bytecode::Interpreter)

ska::bytecode::Interpreter::CommandInterpreter ska::bytecode::Interpreter::build() {
	auto result = CommandInterpreter {};
	static constexpr auto maxCommandEnumIndex = static_cast<std::size_t>(Command::UNUSED_Last_Length);
	result.resize(maxCommandEnumIndex);

	InterpreterCommandDeclare<Command::MOV>(*this, result);

	InterpreterCommandDeclare<Command::ADD_I>(*this, result);
	InterpreterCommandDeclare<Command::SUB_I>(*this, result);
	InterpreterCommandDeclare<Command::MUL_I>(*this, result);
	InterpreterCommandDeclare<Command::DIV_I>(*this, result);

	InterpreterCommandDeclare<Command::ADD_D>(*this, result);
	InterpreterCommandDeclare<Command::SUB_D>(*this, result);
	InterpreterCommandDeclare<Command::MUL_D>(*this, result);
	InterpreterCommandDeclare<Command::DIV_D>(*this, result);

	InterpreterCommandDeclare<Command::ADD_STR>(*this, result);
	InterpreterCommandDeclare<Command::PUSH_ARR_ARR>(*this, result);
	InterpreterCommandDeclare<Command::PUSH_F_ARR>(*this, result);
	InterpreterCommandDeclare<Command::PUSH_B_ARR>(*this, result);
	InterpreterCommandDeclare<Command::SUB_ARR>(*this, result);

	InterpreterCommandDeclare<Command::CONV_D_I>(*this, result);
	InterpreterCommandDeclare<Command::CONV_I_D>(*this, result);
	InterpreterCommandDeclare<Command::CONV_I_STR>(*this, result);
	InterpreterCommandDeclare<Command::CONV_D_STR>(*this, result);

	InterpreterCommandDeclare<Command::CMP_STR>(*this, result);
	InterpreterCommandDeclare<Command::CMP_ARR>(*this, result);
	InterpreterCommandDeclare<Command::TEST_EQ>(*this, result);
	InterpreterCommandDeclare<Command::TEST_G>(*this, result);
	InterpreterCommandDeclare<Command::TEST_GE>(*this, result);
	InterpreterCommandDeclare<Command::TEST_L>(*this, result);
	InterpreterCommandDeclare<Command::TEST_LE>(*this, result);
	InterpreterCommandDeclare<Command::TEST_NEQ>(*this, result);

	InterpreterCommandDeclare<Command::RET>(*this, result);
	InterpreterCommandDeclare<Command::END>(*this, result);
	InterpreterCommandDeclare<Command::PUSH>(*this, result);
	InterpreterCommandDeclare<Command::POP>(*this, result);
	InterpreterCommandDeclare<Command::POP_IN_ARR>(*this, result);
	InterpreterCommandDeclare<Command::POP_IN_VAR>(*this, result);
	InterpreterCommandDeclare<Command::JUMP_ABS>(*this, result);
	InterpreterCommandDeclare<Command::JUMP_REL>(*this, result);

	InterpreterCommandDeclare<Command::ARR_ACCESS>(*this, result);
	InterpreterCommandDeclare<Command::SCRIPT>(*this, result);

	return result;
}

ska::bytecode::Interpreter::Interpreter(const ReservedKeywordsPool& reserved) :
	m_commandInterpreter(build()) {
}

ska::bytecode::ExecutionOutput ska::bytecode::Interpreter::interpret(ExecutionContext& node) {
	auto lastValue = Value{};
	for(auto continueExecution = !node.empty(); continueExecution; continueExecution = node.incInstruction()) {
		auto& instruction = node.currentInstruction();
		LOG_INFO << "Interpreting " << instruction;
		auto& builder = m_commandInterpreter[static_cast<std::size_t>(instruction.command())];
		assert(builder != nullptr);
		auto result = builder->interpret(node);
		if(!result.empty()) {
			lastValue = instruction.dest();
			node.set(instruction.dest(), std::move(result));
		}
	}

	return lastValue.empty() ? ExecutionOutput{} : node.getCell(lastValue);
}

ska::bytecode::ExecutionOutput ska::bytecode::Interpreter::interpret(std::string fullScriptName, GenerationOutput& instructions) {
	LOG_DEBUG << "Interpreting " << instructions;

	auto context = ExecutionContext {m_scripts, std::move(fullScriptName), instructions };
	return interpret(context);
}
