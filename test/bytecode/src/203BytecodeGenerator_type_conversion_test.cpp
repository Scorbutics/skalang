#include <doctest.h>
#include <tuple>
#include "Config/LoggerConfigLang.h"
#include "BytecodeGeneratorTest.h"

TEST_CASE("[BytecodeGenerator] type conversion + int => string") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("var result = 7 + \"3\";");
	auto& res = data.generator->generate(data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::CONV_I_STR, "R0", "7"},
		{ska::bytecode::Command::ADD_STR, "R0", "R0", "3"},
		{ska::bytecode::Command::MOV, "V0", "R0"}
	});
}

/*
// TODO: Unsupported atm (Unable to use operator "+" on types "float" and "string")

TEST_CASE("[BytecodeGenerator] type conversion + float => string") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("var result = 7.0 + \"3\";");
	auto& res = data.generator->generate(data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::CONV_D_STR, "R1", "7.0"},
		{ska::bytecode::Command::ADD_STR, "R0", "R1", "3"},
		{ska::bytecode::Command::MOV, "V0", "R0"}
	});
}
*/

TEST_CASE("[BytecodeGenerator] type conversion + int => float") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("var result = 7.0 + 3;");
	auto& res = data.generator->generate(data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::CONV_I_D, "R0", "3"},
		{ska::bytecode::Command::ADD_D, "R0", "7.000000", "R0"},
		{ska::bytecode::Command::MOV, "V0", "R0"}
	});
}

TEST_CASE("[BytecodeGenerator] type conversion + int => array (back)") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("var result = [7, 12, 25] + 3;");
	auto& res = data.generator->generate(data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::PUSH, "7", "12", "25"},
		{ska::bytecode::Command::POP_IN_ARR, "R0", "3"},
		{ska::bytecode::Command::PUSH_B_ARR, "R1", "R0", "3"},
		{ska::bytecode::Command::MOV, "V0", "R1"}
	});
}

TEST_CASE("[BytecodeGenerator] type conversion + int => array (front)") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("var result = 3 + [7, 12, 25];");
	auto& res = data.generator->generate(data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::PUSH, "7", "12", "25"},
		{ska::bytecode::Command::POP_IN_ARR, "R0", "3"},
		{ska::bytecode::Command::PUSH_F_ARR, "R1", "3", "R0"},
		{ska::bytecode::Command::MOV, "V0", "R1"}
	});
}
