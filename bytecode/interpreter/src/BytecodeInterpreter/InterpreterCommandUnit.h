#pragma once

#include "Value/ExecutionOutput.h"

namespace ska {
	namespace bytecode {
		class Interpreter;
		class ExecutionContext;

		class InterpreterCommandUnit {
		public:
			virtual ExecutionOutput interpret(ExecutionContext& node) = 0;
		};

		class InterpreterCommandBase :
			public InterpreterCommandUnit {
		public:
			InterpreterCommandBase(Interpreter& interpreter) :
				m_interpreter(interpreter) {}
		protected:
			Interpreter& m_interpreter;
		};
	}
}
