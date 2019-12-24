#include <doctest.h>
#include <tuple>
#include "Config/LoggerConfigLang.h"
#include "BytecodeGeneratorTest.h"

using namespace ska::bytecode;

TEST_CASE("[BytecodeGenerator] empty if") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("if( true ) {}");
	auto& res = data.generator->generate(data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ Command::JUMP_NIF, "0", "1" }
	});
}

TEST_CASE("[BytecodeGenerator] if with body") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("if( true ) { var toto = 5; }");
	auto& res = data.generator->generate(data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ Command::JUMP_NIF, "1", "1" },
		{ Command::MOV, "V0", "5" }
	});
}

TEST_CASE("[BytecodeGenerator] if else with body") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("if( 3 > 2 ) { var toto = 1 + 3; } else { var toto = 4 + 2 + 1; var tt = toto; } var tete = 444;");
	auto& res = data.generator->generate(data.storage, std::move(astPtr));

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

TEST_CASE("[BytecodeGenerator] empty for") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("for(;;) {}");
	auto& res = data.generator->generate(data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ Command::JUMP_REL, "-1" }
	});
}

TEST_CASE("[BytecodeGenerator] for without body") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("for(var i = 0; i < 10; i = i + 1);");
	auto& res = data.generator->generate(data.storage, std::move(astPtr));

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
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("for(var i = 0; i < 10; i = i + 1) { var toto = 123; toto + i; } var test = 1234;");
	auto& res = data.generator->generate(data.storage, std::move(astPtr));

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
