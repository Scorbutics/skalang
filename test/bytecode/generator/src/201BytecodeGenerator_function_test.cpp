#include <iostream>
#include <doctest.h>
#include <tuple>
#include "Config/LoggerConfigLang.h"
#include "BytecodeGeneratorTest.h"

TEST_CASE("[BytecodeGenerator] Empty function only void") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("toto = function() do end\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::JUMP_REL, "1"},
		{ska::bytecode::Command::RET},
		{ska::bytecode::Command::END, "V0", "-2" }
	});
}

TEST_CASE("[BytecodeGenerator] Empty function with 1 parameter") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("toto = function(t: int) do end\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::JUMP_REL, "3"},
		{ska::bytecode::Command::POP, "V0"},
		{ska::bytecode::Command::CLEAR_RANGE, "V0", "V0"},
		{ska::bytecode::Command::RET},
		{ska::bytecode::Command::END, "V1", "-4"}
	});
}

TEST_CASE("[BytecodeGenerator] Empty function with 4 parameters (> 3)") {
	static constexpr auto progStr = "toto = function(t: int, t1: string, t2: int, t3: int) do end\n";
	auto [astPtr, data] = ASTFromInputBytecodeGenerator(progStr);
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::JUMP_REL, "4"},
		{ska::bytecode::Command::POP, "V0", "V1", "V2"},
		{ska::bytecode::Command::POP, "V3" },
		{ska::bytecode::Command::CLEAR_RANGE, "V0", "V3" },
		{ska::bytecode::Command::RET},
		{ska::bytecode::Command::END, "V4", "-5"}
	});
}

TEST_CASE("[BytecodeGenerator] Empty function with 4 parameters (> 3) + use") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("toto = function(t: int, t1: int, t2: int, t3: int) do test=t\ntest=t1\ntest=t2\ntest=t3\n end\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::JUMP_REL, "8"},
		{ska::bytecode::Command::POP, "V0", "V1", "V2"},
		{ska::bytecode::Command::POP, "V3" },
		{ska::bytecode::Command::MOV, "V4", "V0" },
		{ska::bytecode::Command::MOV, "V4", "V1" },
		{ska::bytecode::Command::MOV, "V4", "V2" },
		{ska::bytecode::Command::MOV, "V4", "V3" },
		{ska::bytecode::Command::CLEAR_RANGE, "V0", "V4" },
		{ska::bytecode::Command::RET},
		{ska::bytecode::Command::END, "V5", "-9"}
	});
}

TEST_CASE("[BytecodeGenerator] Basic function with 1 return type") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("toto = function(): int do return 0\n end\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::JUMP_REL, "3"},
		{ska::bytecode::Command::MOV, "R0", "0"},
		{ska::bytecode::Command::CLEAR_RANGE, "R0", "R0"},
		{ska::bytecode::Command::RET, "R0"},
		{ska::bytecode::Command::END, "V0", "-4" }
	});
}

TEST_CASE("[BytecodeGenerator] Basic function with 1 parameter 1 return type") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("toto = function(test: int): int do return 0\n end\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::JUMP_REL, "5"},
		{ska::bytecode::Command::POP, "V0"},
		{ska::bytecode::Command::MOV, "R0", "0"},
		{ska::bytecode::Command::CLEAR_RANGE, "R0", "R0"},
		{ska::bytecode::Command::CLEAR_RANGE, "V0", "V0"},
		{ska::bytecode::Command::RET, "R0"},
		{ska::bytecode::Command::END, "V1", "-6" }
	});
}

TEST_CASE("[BytecodeGenerator] Function with 1 parameter and some computing inside") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("toto = function(test: int): int do result = test + 3\n return result\n end\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::JUMP_REL, "7"},
		{ska::bytecode::Command::POP, "V0"},
		{ska::bytecode::Command::ADD_I, "R0", "V0", "3"},
		{ska::bytecode::Command::MOV, "V1", "R0"},
		{ska::bytecode::Command::MOV, "R1", "V1"},
		{ska::bytecode::Command::CLEAR_RANGE, "R1", "R1"},
		{ska::bytecode::Command::CLEAR_RANGE, "V0", "V1"},
		{ska::bytecode::Command::RET, "R1"},
		{ska::bytecode::Command::END, "V2", "-8" }
	});
}


TEST_CASE("[BytecodeGenerator] Custom object creation") {
	constexpr auto progStr =
		"toto = function() : var do\n"
			"priv_test = 1\n"
			"return {"
				"test = priv_test\n"
				"say = function(more : string) : string do\n"
					"s = \"lol\" + priv_test + more\n"
					"return s\n"
				"end\n"
			"}\n"
		"end\n"
		"test = toto()\n";

	auto [astPtr, data] = ASTFromInputBytecodeGenerator(progStr);
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::JUMP_REL, "31"},
		{ska::bytecode::Command::JUMP_REL, "6"},
		{ska::bytecode::Command::MOV, "V0", "1"},
		{ska::bytecode::Command::PUSH, "V0"},
		{ska::bytecode::Command::POP_IN_VAR, "R0", "1"},
		{ska::bytecode::Command::CLEAR_RANGE, "R0", "R0"},
		{ska::bytecode::Command::CLEAR_RANGE, "V0", "V0"},
		{ska::bytecode::Command::RET, "R0"},
		{ska::bytecode::Command::END, "V1", "-7"},
		{ska::bytecode::Command::JUMP_ABS, "V1"},
		{ska::bytecode::Command::POP, "R1"},
		{ska::bytecode::Command::MOV, "V2", "R1"},
		{ska::bytecode::Command::PUSH, "V2"},
		{ska::bytecode::Command::MOV, "V3", "V0"},
		{ska::bytecode::Command::PUSH, "V3"},
		{ska::bytecode::Command::JUMP_REL, "10"},
		{ska::bytecode::Command::POP, "V4", "V5"},
		{ska::bytecode::Command::ARR_ACCESS, "R2", "V4", "0"},
		{ska::bytecode::Command::CONV_I_STR, "R3", "R2"},
		{ska::bytecode::Command::ADD_STR, "R3", "R3", "V5"},
		{ska::bytecode::Command::ADD_STR, "R4", "lol", "R3"},
		{ska::bytecode::Command::MOV, "V6", "R4"},
		{ska::bytecode::Command::MOV, "R5", "V6"},
		{ska::bytecode::Command::CLEAR_RANGE, "R2", "R5"},
		{ska::bytecode::Command::CLEAR_RANGE, "V4", "V6"},
		{ska::bytecode::Command::RET, "R5"},
		{ska::bytecode::Command::END, "V7", "-11"},
		{ska::bytecode::Command::PUSH, "V7"},
		{ska::bytecode::Command::POP_IN_VAR, "R6", "3"},
		{ska::bytecode::Command::CLEAR_RANGE, "R0", "R6"},
		{ska::bytecode::Command::CLEAR_RANGE, "V0", "V7"},
		{ska::bytecode::Command::RET, "R6"},
		{ska::bytecode::Command::END, "V8", "-32"},
		{ska::bytecode::Command::JUMP_ABS, "V8"},
		{ska::bytecode::Command::POP, "R7"},
		{ska::bytecode::Command::MOV, "V9", "R7"}
	});
}

TEST_CASE("[BytecodeGenerator] Use complex call inside function parameters") {
	constexpr auto progStr =
		"titi = function() : var do return { test = \"tulululu\" }\n end\n"
		"toto = function(arg1 : string) : void do end\n"
		"toto(titi().test)\n";

	auto [astPtr, data] = ASTFromInputBytecodeGenerator(progStr);
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::JUMP_REL, "15"},
		{ska::bytecode::Command::JUMP_REL, "3"},
		{ska::bytecode::Command::POP_IN_VAR, "R0", "0"},
		{ska::bytecode::Command::CLEAR_RANGE, "R0", "R0"},
		{ska::bytecode::Command::RET, "R0"},
		{ska::bytecode::Command::END, "V0", "-4"},
		{ska::bytecode::Command::JUMP_ABS, "V0"},
		{ska::bytecode::Command::POP, "R1"},
		{ska::bytecode::Command::MOV, "V1", "R1"},
		{ska::bytecode::Command::PUSH, "V1"},
		{ska::bytecode::Command::MOV, "V2", "tulululu"},
		{ska::bytecode::Command::PUSH, "V2"},
		{ska::bytecode::Command::POP_IN_VAR, "R2", "2"},
		{ska::bytecode::Command::CLEAR_RANGE, "R0", "R2"},
		{ska::bytecode::Command::CLEAR_RANGE, "V0", "V2"},
		{ska::bytecode::Command::RET, "R2"},
		{ska::bytecode::Command::END, "V3", "-16"},
		{ska::bytecode::Command::JUMP_REL, "3"},
		{ska::bytecode::Command::POP, "V4"},
		{ska::bytecode::Command::CLEAR_RANGE, "V4", "V4"},
		{ska::bytecode::Command::RET },
		{ska::bytecode::Command::END, "V5", "-4"},
		{ska::bytecode::Command::JUMP_ABS, "V3"},
		{ska::bytecode::Command::POP, "R3"},
		{ska::bytecode::Command::ARR_ACCESS, "R4", "R3", "1"},
		{ska::bytecode::Command::PUSH, "R4"},
		{ska::bytecode::Command::JUMP_ABS, "V5"}
	});
}

TEST_CASE("[BytecodeGenerator] Custom object creation2") {
	constexpr auto progStr =
		"toto = function() : var do\n"
			"priv_test = 123\n"
			"return {"
				"test = priv_test\n"
				"say = function(more : string) : string do\n"
					"s = \"lol\" + priv_test + more\n"
					"return s\n"
				"end"
			"}\n"
		"end\n"
		"test = toto()\n"
		"test.say(\"titi\")\n";

	auto [script, data] = ASTFromInputBytecodeGenerator(progStr);
	auto& res = data.generator->generate(*data.storage, std::move(script));

	BytecodeCompare(res, {
		{ska::bytecode::Command::JUMP_REL, "31"},
		{ska::bytecode::Command::JUMP_REL, "6"},
		{ska::bytecode::Command::MOV, "V0", "123"},
		{ska::bytecode::Command::PUSH, "V0"},
		{ska::bytecode::Command::POP_IN_VAR, "R0", "1"},
		{ska::bytecode::Command::CLEAR_RANGE, "R0", "R0"},
		{ska::bytecode::Command::CLEAR_RANGE, "V0", "V0"},
		{ska::bytecode::Command::RET, "R0"},
		{ska::bytecode::Command::END, "V1", "-7"},
		{ska::bytecode::Command::JUMP_ABS, "V1"},
		{ska::bytecode::Command::POP, "R1"},
		{ska::bytecode::Command::MOV, "V2", "R1"},
		{ska::bytecode::Command::PUSH, "V2"},
		{ska::bytecode::Command::MOV, "V3", "V0"},
		{ska::bytecode::Command::PUSH, "V3"},
		{ska::bytecode::Command::JUMP_REL, "10"},
		{ska::bytecode::Command::POP, "V4", "V5"},
		{ska::bytecode::Command::ARR_ACCESS, "R2", "V4", "0"},
		{ska::bytecode::Command::CONV_I_STR, "R3", "R2"},
		{ska::bytecode::Command::ADD_STR, "R3", "R3", "V5"},
		{ska::bytecode::Command::ADD_STR, "R4", "lol", "R3"},
		{ska::bytecode::Command::MOV, "V6", "R4"},
		{ska::bytecode::Command::MOV, "R5", "V6"},
		{ska::bytecode::Command::CLEAR_RANGE, "R2", "R5"},
		{ska::bytecode::Command::CLEAR_RANGE, "V4", "V6"},
		{ska::bytecode::Command::RET, "R5"},
		{ska::bytecode::Command::END, "V7", "-11"},
		{ska::bytecode::Command::PUSH, "V7"},
		{ska::bytecode::Command::POP_IN_VAR, "R6", "3"},
		{ska::bytecode::Command::CLEAR_RANGE, "R0", "R6"},
		{ska::bytecode::Command::CLEAR_RANGE, "V0", "V7"},
		{ska::bytecode::Command::RET, "R6"},
		{ska::bytecode::Command::END, "V8", "-32"},
		{ska::bytecode::Command::JUMP_ABS, "V8"},
		{ska::bytecode::Command::POP, "R7"},
		{ska::bytecode::Command::MOV, "V9", "R7"},
		{ska::bytecode::Command::ARR_MEMBER_ACCESS, "R8", "V9", "2"},
		{ska::bytecode::Command::PUSH, "titi"},
		{ska::bytecode::Command::JUMP_MEMBER, "R8"},
		{ska::bytecode::Command::POP, "R9"}
	});
}

// We have to check that calling a function several times stills refer to the same
// JUMP_ABS instruction
TEST_CASE("[BytecodeInterpreter] Custom object creation 3 (double field function call)") {
	constexpr auto progStr =
		"toto = function() : var do\n"
			"priv_test = 123\n"
			"return {"
				"test = priv_test\n"
				"say = function(more : string) : string do\n"
					"s = \"lol\" + priv_test + more\n"
					"return s\n"
				"end\n"
			"}\n"
		"end\n"
		"test = toto()\n"
		"test.say(\"titi\")\n"
		"test.say(\"titi4\")\n";

	auto [script, data] = ASTFromInputBytecodeGenerator(progStr);
	auto& res = data.generator->generate(*data.storage, std::move(script));

	BytecodeCompare(res, {
		{ska::bytecode::Command::JUMP_REL, "31"},
		{ska::bytecode::Command::JUMP_REL, "6"},
		{ska::bytecode::Command::MOV, "V0", "123"},
		{ska::bytecode::Command::PUSH, "V0"},
		{ska::bytecode::Command::POP_IN_VAR, "R0", "1"},
		{ska::bytecode::Command::CLEAR_RANGE, "R0", "R0"},
		{ska::bytecode::Command::CLEAR_RANGE, "V0", "V0"},
		{ska::bytecode::Command::RET, "R0"},
		{ska::bytecode::Command::END, "V1", "-7"},
		{ska::bytecode::Command::JUMP_ABS, "V1"},
		{ska::bytecode::Command::POP, "R1"},
		{ska::bytecode::Command::MOV, "V2", "R1"},
		{ska::bytecode::Command::PUSH, "V2"},
		{ska::bytecode::Command::MOV, "V3", "V0"},
		{ska::bytecode::Command::PUSH, "V3"},
		{ska::bytecode::Command::JUMP_REL, "10"},
		{ska::bytecode::Command::POP, "V4", "V5"},
		{ska::bytecode::Command::ARR_ACCESS, "R2", "V4", "0"},
		{ska::bytecode::Command::CONV_I_STR, "R3", "R2"},
		{ska::bytecode::Command::ADD_STR, "R3", "R3", "V5"},
		{ska::bytecode::Command::ADD_STR, "R4", "lol", "R3"},
		{ska::bytecode::Command::MOV, "V6", "R4"},
		{ska::bytecode::Command::MOV, "R5", "V6"},
		{ska::bytecode::Command::CLEAR_RANGE, "R2", "R5"},
		{ska::bytecode::Command::CLEAR_RANGE, "V4", "V6"},
		{ska::bytecode::Command::RET, "R5"},
		{ska::bytecode::Command::END, "V7", "-11"},
		{ska::bytecode::Command::PUSH, "V7"},
		{ska::bytecode::Command::POP_IN_VAR, "R6", "3"},
		{ska::bytecode::Command::CLEAR_RANGE, "R0", "R6"},
		{ska::bytecode::Command::CLEAR_RANGE, "V0", "V7"},
		{ska::bytecode::Command::RET, "R6"},
		{ska::bytecode::Command::END, "V8", "-32"},
		{ska::bytecode::Command::JUMP_ABS, "V8"},
		{ska::bytecode::Command::POP, "R7"},
		{ska::bytecode::Command::MOV, "V9", "R7"},
		{ska::bytecode::Command::ARR_MEMBER_ACCESS, "R8", "V9", "2"},
		{ska::bytecode::Command::PUSH, "titi"},
		{ska::bytecode::Command::JUMP_MEMBER, "R8"},
		{ska::bytecode::Command::POP, "R9"},
		{ska::bytecode::Command::ARR_MEMBER_ACCESS, "R10", "V9", "2"},
		{ska::bytecode::Command::PUSH, "titi4"},
		{ska::bytecode::Command::JUMP_MEMBER, "R10"},
		{ska::bytecode::Command::POP, "R11"}
	});
}

TEST_CASE("[BytecodeGenerator] Field access affectation") {
	constexpr auto progStr =
		"toto = function() : var do\n"
			"priv_test = 123\n"
			"return {"
				"test = priv_test\n"
				"say = function(more : string) : string do\n"
					"s = \"lol\" + priv_test + more\n"
					"return s\n"
				"end"
			"}\n"
		"end\n"
		"t = toto()\n"
		"t.test = 1122\n";
	auto [astPtr, data] = ASTFromInputBytecodeGenerator(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(astPtr));
}

TEST_CASE("[BytecodeGenerator] everything inside factory function is relative to object instance") {
	constexpr auto progStr =
	"TestFcty = function(value_: string): var do\n"
		"return {\n"
			"value = function(): string do\n"
				"return value_\n"
			"end\n"
		"}\n"
	"end\n"

	"toto = TestFcty(\"toto\")\n"
	"titi = TestFcty(\"titi\")\n"

	"toutou = toto.value\n"
	"if true\n"
	"toutou = titi.value\n"
	"end\n"
	"toutou()\n";

	// We expect toto.value() == "toto" and titi.value() == "titi"
	auto [astPtr, data] = ASTFromInputBytecodeGenerator(progStr);
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

#ifndef NDEBUG
	ska::bytecode::InstructionsDebugInfo{ progStr, 50 }.print(std::cout, *data.storage, res.id());
#endif

	BytecodeCompare(res, {
		{ska::bytecode::Command::JUMP_REL, "25"},
		{ska::bytecode::Command::POP, "V0"},
		{ska::bytecode::Command::JUMP_REL, "6"},
		{ska::bytecode::Command::MOV, "V1", "V0"},
		{ska::bytecode::Command::PUSH, "V1"},
		{ska::bytecode::Command::POP_IN_VAR, "R0", "1"},
		{ska::bytecode::Command::CLEAR_RANGE, "R0", "R0"},
		{ska::bytecode::Command::CLEAR_RANGE, "V1", "V1"},
		{ska::bytecode::Command::RET, "R0" },
		{ska::bytecode::Command::END, "V2", "-7" },
		{ska::bytecode::Command::JUMP_ABS, "V2" },
		{ska::bytecode::Command::POP, "R1" },
		{ska::bytecode::Command::MOV, "V3", "R1" },
		{ska::bytecode::Command::PUSH, "V3" },
		{ska::bytecode::Command::JUMP_REL, "5" },
		{ska::bytecode::Command::POP, "V4" },
		{ska::bytecode::Command::ARR_ACCESS, "R2", "V4", "0" },
		{ska::bytecode::Command::CLEAR_RANGE, "R2", "R2"},
		{ska::bytecode::Command::CLEAR_RANGE, "V4", "V4"},
		{ska::bytecode::Command::RET, "R2" },
		{ska::bytecode::Command::END, "V5", "-6" },
		{ska::bytecode::Command::PUSH, "V5" },
		{ska::bytecode::Command::POP_IN_VAR, "R4", "2" },
		{ska::bytecode::Command::CLEAR_RANGE, "R0", "R4"},
		{ska::bytecode::Command::CLEAR_RANGE, "V0", "V5"},
		{ska::bytecode::Command::RET, "R4" },
		{ska::bytecode::Command::END, "V6", "-26" },
		{ska::bytecode::Command::PUSH, "toto" },
		{ska::bytecode::Command::JUMP_ABS, "V6" },
		{ska::bytecode::Command::POP, "R5" },
		{ska::bytecode::Command::MOV, "V7", "R5" },
		{ska::bytecode::Command::PUSH, "titi" },
		{ska::bytecode::Command::JUMP_ABS, "V6" },
		{ska::bytecode::Command::POP, "R6" },
		{ska::bytecode::Command::MOV, "V8", "R6" },
		{ska::bytecode::Command::ARR_MEMBER_ACCESS, "R7", "V7", "1" },
		{ska::bytecode::Command::MOV, "V9", "R7" },
		{ska::bytecode::Command::JUMP_NIF, "2", "1" },
		{ska::bytecode::Command::ARR_MEMBER_ACCESS, "R8", "V8", "1" },
		{ska::bytecode::Command::MOV, "V9", "R8" },
		{ska::bytecode::Command::JUMP_MEMBER, "V9" },
		{ska::bytecode::Command::POP, "R9" }
	});
}

TEST_CASE("[BytecodeGenerator] callback array") {
	constexpr auto progStr =
		"Default = function() do end\n"

		"call = function(callback: Default[]) do\n"
			"callback[0]()\n"
		"end\n"

		"call([Default])\n";
	auto [astPtr, data] = ASTFromInputBytecodeGenerator(progStr);
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::JUMP_REL, "1" },
		{ska::bytecode::Command::RET },
		{ska::bytecode::Command::END, "V0", "-2" },
		{ska::bytecode::Command::JUMP_REL, "6" },
		{ska::bytecode::Command::POP, "V1" },
		{ska::bytecode::Command::ARR_ACCESS, "R0", "V1", "0" },
		{ska::bytecode::Command::JUMP_ABS, "R0" },
		{ska::bytecode::Command::CLEAR_RANGE, "R0", "R0" },
		{ska::bytecode::Command::CLEAR_RANGE, "V1", "V1" },
		{ska::bytecode::Command::RET },
		{ska::bytecode::Command::END, "V2", "-7" },
		{ska::bytecode::Command::PUSH, "V0" },
		{ska::bytecode::Command::POP_IN_ARR, "R1", "1" },
		{ska::bytecode::Command::PUSH, "R1" },
		{ska::bytecode::Command::JUMP_ABS, "V2" }
	});
}
