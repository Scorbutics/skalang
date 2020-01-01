#include <doctest.h>
#include <tuple>
#include "Config/LoggerConfigLang.h"

#include "BytecodeInterpreter/BytecodeScript.h"
#include "BytecodeInterpreter/Value/BytecodeInterpreterTypes.h"
#include "BytecodeInterpreter/BytecodeInterpreter.h"

#include "BytecodeInterpreterTest.h"

#include "Runtime/Value/InterpreterTypes.h"
#include "std/module/io/path.h"

static ska::bytecode::ScriptGenerationHelper Interpret(BytecodeInterpreterDataTestContainer& data, const std::string& input) {
	readerI->parse(*data.parser);
	return ska::bytecode::ScriptGenerationHelper{*data.storage, *readerI };
}

TEST_CASE("[BytecodeInterpreter] Binding std : path import only") {
	constexpr auto progStr =
	"var PathFcty = import \"bind:std.native.io.path\";"
	"var path = PathFcty.Fcty(\"" SKALANG_TEST_DIR "\");";

	auto data = BytecodeInterpreterDataTestContainer{};
	ASTFromInputBytecodeInterpreterNoParse(progStr, data);

	auto moduleConfiguration = ska::lang::ModuleConfiguration<ska::bytecode::Interpreter> { data.storage->astCache, *data.storage, *data.typeBuilder, *data.symbolsTypeUpdater, reservedKeywords, *data.parser, *data.interpreter};
	auto pathmodule = ska::lang::IOPathModule(moduleConfiguration);

	auto script = Interpret(data, progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
}

TEST_CASE("[BytecodeInterpreter] Binding std : path + bridge function call") {
	constexpr auto progStr =
	"var PathFcty = import \"bind:std.native.io.path\";"
	"var path = PathFcty.Fcty(\"\");"
	"var last = path.canonical();";

	auto data = BytecodeInterpreterDataTestContainer{};
	ASTFromInputBytecodeInterpreterNoParse(progStr, data);

	auto moduleConfiguration = ska::lang::ModuleConfiguration<ska::bytecode::Interpreter> { data.storage->astCache, *data.storage, *data.typeBuilder, *data.symbolsTypeUpdater, reservedKeywords, *data.parser, *data.interpreter};
	auto pathmodule = ska::lang::IOPathModule(moduleConfiguration);

	auto script = Interpret(data, progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(gen.id());
	auto cellValue = *res.nodeval<ska::StringShared>();
	CHECK(cellValue == "");
}
