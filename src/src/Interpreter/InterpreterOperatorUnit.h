#pragma once

#include "NodeValue/Token.h"
#include "NodeCell.h"

namespace ska {
	class ASTNode;
	class Interpreter;
	class ExecutionContext;

	class InterpreterOperatorUnit {
	public:
		virtual NodeCell interpret(ExecutionContext& node) = 0;
	};

	class InterpreterOperatorBase :
		public InterpreterOperatorUnit {
	public:
		InterpreterOperatorBase(Interpreter& interpreter) :
			m_interpreter(interpreter) {}
	protected:
		Interpreter& m_interpreter;
	};

}
