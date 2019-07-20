#pragma once

namespace ska {
	enum class BytecodeCommand {
		SET,
		IN,
		OUT,
		PUSH,
		POP,
		LABEL,
		JUMP,
		FIELD,
		SCRIPT,
		END,
		MUL,
		ADD,
		DIV,
		SUB,
		NOP
	};
}