#include <doctest.h>
#include <tuple>
#include "Config/LoggerConfigLang.h"
#include "BytecodeGeneratorTest.h"

using namespace ska::bytecode;

TEST_CASE("[BytecodeGenerator] empty if") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("if( true ) do end");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ Command::JUMP_NIF, "0", "1" }
	});
}

TEST_CASE("[BytecodeGenerator] if with body") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("if( true ) do toto = 5\n end");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ Command::JUMP_NIF, "1", "1" },
		{ Command::MOV, "V0", "5" }
	});
}

TEST_CASE("[BytecodeGenerator] if else with body") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("if( 3 > 2 ) do toto = 1 + 3\n end else do toto = 4 + 2 + 1\n tt = toto\n end tete = 444\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ Command::SUB_I, "R0", "3", "2" },
		{ Command::TEST_G, "R0", "R0" },
		{ Command::JUMP_NIF, "3", "R0" },
		{ Command::ADD_I, "R1", "1", "3" },
		{ Command::MOV, "V0", "R1" },
		{ Command::JUMP_REL, "4" },
		{ Command::ADD_I, "R2", "2", "1" },
		{ Command::ADD_I, "R3", "4", "R2" },
		{ Command::MOV, "V1", "R3" },
		{ Command::MOV, "V2", "V1" },
		{ Command::MOV, "V3", "444" }
	});

}

TEST_CASE("[BytecodeGenerator] for with empty body") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("for(i = 0\n i < 10\n i = i + 1) do end");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		// Initialization part
		{ Command::MOV, "V0", "0" },
		
		// Check part
		{ Command::SUB_I, "R0", "V0", "10" },
		{ Command::TEST_L, "R0", "R0" },
		{ Command::JUMP_NIF, "3", "R0" },
		
		// Body part
		
		// Increment part
		{ Command::ADD_I, "R1", "V0", "1"},
		{ Command::MOV, "V0", "R1" },
		{ Command::JUMP_REL, "-6" }
	});
}

TEST_CASE("[BytecodeGenerator] for with body") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("for(i = 0\n i < 10\n i = i + 1) do toto = 123\n toto + i\n end test = 1234\n");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		// Initialization part
		{ Command::MOV, "V0", "0" },

		// Check part
		{ Command::SUB_I, "R0", "V0", "10" },
		{ Command::TEST_L, "R0", "R0" },
		{ Command::JUMP_NIF, "5", "R0" },
		
		// Body part
		{ Command::MOV, "V1", "123" },
		{ Command::ADD_I, "R1", "V1", "V0" },

		// Increment part
		{ Command::ADD_I, "R2", "V0", "1"},
		{ Command::MOV, "V0", "R2" },
		{ Command::JUMP_REL, "-8" },

		// Post part
		{ Command::MOV, "V2", "1234"}
	});
}
