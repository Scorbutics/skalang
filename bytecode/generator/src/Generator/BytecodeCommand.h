#pragma once
#include <unordered_map>
#include <ostream>

namespace ska {
	namespace bytecode {
		enum class Command {
			MOV,
			CONV_I_STR,
			CONV_D_STR,
			CONV_I_D,
			CONV_D_I,
			PUSH_F_ARR,
			PUSH_ARR_ARR,
			PUSH_B_ARR,
			POP_F_ARR,
			POP_B_ARR,
			POP_IN_ARR,
			POP_IN_VAR,
			ARR_ACCESS,
			JUMP_REL,
			JUMP_ABS,
			JUMP_NIF,
			PUSH,
			POP,
			FIELD,
			SCRIPT,
			RET,
			END,
			MUL_I,
			MUL_I_STR,
			MUL_STR_I,
			MUL_I_ARR,
			MUL_ARR_I,
			MUL_D,
			ADD_I,
			ADD_STR,
			ADD_D,
			DIV_I,
			DIV_D,
			SUB_I,
			SUB_D,
			SUB_ARR,
			CMP_STR,
			CMP_ARR,
			TEST_EQ,
			TEST_NEQ,
			TEST_G,
			TEST_GE,
			TEST_L,
			TEST_LE,
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
			"PUSH_ARR_ARR",
			"PUSH_B_ARR",
			"POP_F_ARR",
			"POP_B_ARR",
			"POP_IN_ARR",
			"POP_IN_VAR",
			"ARR_ACCESS",
			"JUMP_REL",
			"JUMP_ABS",
			"JUMP_NIF",
			"PUSH",
			"POP",
			"FIELD",
			"SCRIPT",
			"RET",
			"END",
			"MUL_I",
			"MUL_I_STR",
			"MUL_STR_I",
			"MUL_I_ARR",
			"MUL_ARR_I",
			"MUL_D",
			"ADD_I",
			"ADD_STR",
			"ADD_D",
			"DIV_I",
			"DIV_D",
			"SUB_I",
			"SUB_D",
			"SUB_ARR",
			"CMP_STR",
			"CMP_ARR",
			"TEST_EQ",
			"TEST_NEQ",
			"TEST_G",
			"TEST_GE",
			"TEST_L",
			"TEST_LE",
			"NOP",
			""
		};

		static_assert(sizeof(CommandSTR) / sizeof(CommandSTR[0]) == (static_cast<std::size_t>(Command::UNUSED_Last_Length) + 1));

		static inline std::ostream& operator<<(std::ostream& stream, const Command& cmd) {
			stream << CommandSTR[static_cast<std::size_t>(cmd)];
			return stream;
		}

	}
}