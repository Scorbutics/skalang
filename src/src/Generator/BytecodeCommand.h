#pragma once
#include <unordered_map>

namespace ska {
	namespace bytecode {
		enum class Command {
			MOV,
			CONV,
			LABEL,
			JUMP,
			PUSH,
			POP,
			FIELD,
			SCRIPT,
			END,
			MUL,
			ADD,
			DIV,
			SUB,
			NOP
		};

		static constexpr const char* CommandSTR[] = {
			"MOV",
			"CONV",
			"LABEL",
			"JUMP",
			"PUSH",
			"POP",
			"FIELD",
			"SCRIPT",
			"END",
			"MUL",
			"ADD",
			"DIV",
			"SUB",
			"NOP"
		};

		static const auto CommandMap = std::unordered_map<std::string, Command> {
			{"+",  Command::ADD},
			{"*",  Command::MUL},
			{"/",  Command::DIV},
			{"-",  Command::SUB}
		};
	}
}