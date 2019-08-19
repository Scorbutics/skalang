#pragma once

#include "Generator/BytecodeCommand.h"
#include "BytecodeInterpreter.h"
#include "InterpreterCommandUnit.h"
#include "BytecodeExecutionContext.h"

namespace ska {
	namespace bytecode {
		template <Command cmd>
		class InterpreterCommand;
	}
}

#define SKALANG_BYTECODE_INTERPRETER_COMMAND_DEFINE(CommandType)\
	template<>\
	class InterpreterCommand<CommandType> :\
		public InterpreterCommandBase {\
	public:\
		using InterpreterCommandBase::InterpreterCommandBase;\
		ExecutionOutput interpret(ExecutionContext& context) override final;\
	};

#define SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(CommandType)\
	ska::bytecode::ExecutionOutput ska::bytecode::InterpreterCommand<ska::bytecode::Command::CommandType>::interpret(ExecutionContext& context)
