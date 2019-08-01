#pragma once
#include <unordered_map>

namespace ska {
	namespace bytecode {
		enum class Command {
			MOV,
			CONV_I_STR,
			CONV_D_STR,
			CONV_I_D,
			CONV_D_I,
			PUSH_F_ARR,
			PUSH_F_ARR_ARR,
			PUSH_B_ARR,
			PUSH_B_ARR_ARR,
			POP_F_ARR,
			POP_B_ARR,
			LABEL,
			JUMP,
			PUSH,
			POP,
			FIELD,
			SCRIPT,
			END,
			MUL_I,
			MUL_I_STR,
			MUL_I_ARR,
			MUL_D,
			ADD_I,
			ADD_STR,
			ADD_D,
			DIV_I,
			DIV_D,
			SUB_I,
			SUB_D,
			SUB_ARR,
			NOP,
			UNUSED_Last_Length
		};

		static constexpr const char* CommandSTR[] = {
			"MOV",
			"CONV_I_STR",
			"CONV_D_STR",
			"CONV_I_D",
			"CONV_D_I",
			"PUSH_F_ARR",
			"PUSH_F_ARR_ARR",
			"PUSH_B_ARR",
			"PUSH_B_ARR_ARR",
			"POP_F_ARR",
			"POP_B_ARR",
			"LABEL",
			"JUMP",
			"PUSH",
			"POP",
			"FIELD",
			"SCRIPT",
			"END",
			"MUL_I",
			"MUL_I_STR",
			"MUL_I_ARR",
			"MUL_D",
			"ADD_I",
			"ADD_STR",
			"ADD_D",
			"DIV_I",
			"DIV_D",
			"SUB_I",
			"SUB_D",
			"SUB_ARR",
			"NOP",
			"UNUSED_Last_Length"
		};

		static const auto CommandMap = std::unordered_map<std::string, Command> {
			{"+",  Command::ADD_I},
			{"*",  Command::MUL_I},
			{"/",  Command::DIV_I},
			{"-",  Command::SUB_I}
		};
	}
}