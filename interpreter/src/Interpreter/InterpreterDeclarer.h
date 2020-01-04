#pragma once
#include <memory>
#include <vector>

#include "InterpreterOperatorUnit.h"
#include "NodeValue/Operator.h"

namespace ska {
	class Interpreter;
	template <Operator op, class ... Args>
	static void InterpreterOperatorDeclare(Interpreter& interpreter, std::vector<std::unique_ptr<InterpreterOperatorUnit>>& target, Args&& ... args) {
	target[static_cast<std::size_t>(op)] = std::make_unique<InterpreterOperator<op>>(interpreter, std::forward<Args>(args)...);
	}
}
