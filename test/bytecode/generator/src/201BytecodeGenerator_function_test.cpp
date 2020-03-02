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
		{ska::bytecode::Command::JUMP_REL, "2"},
		{ska::bytecode::Command::POP, "V0"},
		{ska::bytecode::Command::RET},
		{ska::bytecode::Command::END, "V1", "-3"}
	});
}

TEST_CASE("[BytecodeGenerator] Empty function with 4 parameters (> 3)") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("toto = function(t: int, t1: string, t2: int, t3: int) do end\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::JUMP_REL, "3"},
		{ska::bytecode::Command::POP, "V0", "V1", "V2"},
		{ska::bytecode::Command::POP, "V3" },
		{ska::bytecode::Command::RET},
		{ska::bytecode::Command::END, "V4", "-4"}
	});
}

TEST_CASE("[BytecodeGenerator] Basic function with 1 return type") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("toto = function(): int do return 0\n end\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::JUMP_REL, "1"},
		{ska::bytecode::Command::RET, "0"},
		{ska::bytecode::Command::END, "V0", "-2" }
	});
}

TEST_CASE("[BytecodeGenerator] Basic function with 1 parameter 1 return type") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("toto = function(test: int): int do return 0\n end\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::JUMP_REL, "2"},
		{ska::bytecode::Command::POP, "V0"},
		{ska::bytecode::Command::RET, "0"},
		{ska::bytecode::Command::END, "V1", "-3" }
	});
}

TEST_CASE("[BytecodeGenerator] Function with 1 parameter and some computing inside") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("toto = function(test: int): int do result = test + 3\n return result\n end\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::JUMP_REL, "4"},
		{ska::bytecode::Command::POP, "V0"},
		{ska::bytecode::Command::ADD_I, "R0", "V0", "3"},
		{ska::bytecode::Command::MOV, "V1", "R0"},
		{ska::bytecode::Command::RET, "V1"},
		{ska::bytecode::Command::END, "V2", "-5" }
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
		{ska::bytecode::Command::JUMP_REL, "14"},
		{ska::bytecode::Command::MOV, "V0", "1"},
		{ska::bytecode::Command::MOV, "V1", "V0"},
		{ska::bytecode::Command::PUSH, "V1"},
		{ska::bytecode::Command::JUMP_REL, "6"},
		{ska::bytecode::Command::POP, "V2"},
		{ska::bytecode::Command::CONV_I_STR, "R0", "V0"},
		{ska::bytecode::Command::ADD_STR, "R0", "R0", "V2"},
		{ska::bytecode::Command::ADD_STR, "R1", "lol", "R0"},
		{ska::bytecode::Command::MOV, "V3", "R1"},
		{ska::bytecode::Command::RET, "V3"},
		{ska::bytecode::Command::END, "V4", "-7"},
		{ska::bytecode::Command::PUSH, "V4"},
		{ska::bytecode::Command::POP_IN_VAR, "R2", "2"},
		{ska::bytecode::Command::RET, "R2"},
		{ska::bytecode::Command::END, "V5", "-15"},
		{ska::bytecode::Command::JUMP_ABS, "V5"},
		{ska::bytecode::Command::POP, "R3"},
		{ska::bytecode::Command::MOV, "V6", "R3"}
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
		{ska::bytecode::Command::JUMP_REL, "4"},
		{ska::bytecode::Command::MOV, "V0", "tulululu"},
		{ska::bytecode::Command::PUSH, "V0"},
		{ska::bytecode::Command::POP_IN_VAR, "R0", "1"},
		{ska::bytecode::Command::RET, "R0"},
		{ska::bytecode::Command::END, "V1", "-5"},
		{ska::bytecode::Command::JUMP_REL, "2"},
		{ska::bytecode::Command::POP, "V2"},
		{ska::bytecode::Command::RET },
		{ska::bytecode::Command::END, "V3", "-3"},
		{ska::bytecode::Command::JUMP_ABS, "V1"},
		{ska::bytecode::Command::POP, "R1"},
		{ska::bytecode::Command::ARR_ACCESS, "R2", "R1", "0"},
		{ska::bytecode::Command::PUSH, "R2"},
		{ska::bytecode::Command::JUMP_ABS, "V3"}
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
		{ska::bytecode::Command::JUMP_REL, "14"},
		{ska::bytecode::Command::MOV, "V0", "123"},
		{ska::bytecode::Command::MOV, "V1", "V0"},
		{ska::bytecode::Command::PUSH, "V1"},
		{ska::bytecode::Command::JUMP_REL, "6"},
		{ska::bytecode::Command::POP, "V2"},
		{ska::bytecode::Command::CONV_I_STR, "R0", "V0"},
		{ska::bytecode::Command::ADD_STR, "R0", "R0", "V2"},
		{ska::bytecode::Command::ADD_STR, "R1", "lol", "R0"},
		{ska::bytecode::Command::MOV, "V3", "R1"},
		{ska::bytecode::Command::RET, "V3"},
		{ska::bytecode::Command::END, "V4", "-7"},
		{ska::bytecode::Command::PUSH, "V4"},
		{ska::bytecode::Command::POP_IN_VAR, "R2", "2"},
		{ska::bytecode::Command::RET, "R2"},
		{ska::bytecode::Command::END, "V5", "-15"},
		{ska::bytecode::Command::JUMP_ABS, "V5"},
		{ska::bytecode::Command::POP, "R3"},
		{ska::bytecode::Command::MOV, "V6", "R3"},
		{ska::bytecode::Command::ARR_ACCESS, "R4", "V6", "1"},
		{ska::bytecode::Command::PUSH, "titi"},
		{ska::bytecode::Command::JUMP_ABS, "R4"},
		{ska::bytecode::Command::POP, "R5"}
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
		{ska::bytecode::Command::JUMP_REL, "14"},
		{ska::bytecode::Command::MOV, "V0", "123"},
		{ska::bytecode::Command::MOV, "V1", "V0"},
		{ska::bytecode::Command::PUSH, "V1"},
		{ska::bytecode::Command::JUMP_REL, "6"},
		{ska::bytecode::Command::POP, "V2"},
		{ska::bytecode::Command::CONV_I_STR, "R0", "V0"},
		{ska::bytecode::Command::ADD_STR, "R0", "R0", "V2"},
		{ska::bytecode::Command::ADD_STR, "R1", "lol", "R0"},
		{ska::bytecode::Command::MOV, "V3", "R1"},
		{ska::bytecode::Command::RET, "V3"},
		{ska::bytecode::Command::END, "V4", "-7"},
		{ska::bytecode::Command::PUSH, "V4"},
		{ska::bytecode::Command::POP_IN_VAR, "R2", "2"},
		{ska::bytecode::Command::RET, "R2"},
		{ska::bytecode::Command::END, "V5", "-15"},
		{ska::bytecode::Command::JUMP_ABS, "V5"},
		{ska::bytecode::Command::POP, "R3"},
		{ska::bytecode::Command::MOV, "V6", "R3"},
		{ska::bytecode::Command::ARR_ACCESS, "R4", "V6", "1"},
		{ska::bytecode::Command::PUSH, "titi"},
		{ska::bytecode::Command::JUMP_ABS, "R4"},
		{ska::bytecode::Command::POP, "R5"},
		{ska::bytecode::Command::ARR_ACCESS, "R6", "V6", "1"},
		{ska::bytecode::Command::PUSH, "titi4"},
		{ska::bytecode::Command::JUMP_ABS, "R6"},
		{ska::bytecode::Command::POP, "R7"}
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
