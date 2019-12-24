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
	ASTFromInputBytecodeInterpreterNoParse(input, data);
	readerI->parse(*data.parser);
	return ska::bytecode::ScriptGenerationHelper{data.storage, *readerI };
}

TEST_CASE("[BytecodeInterpreter] Binding std : path") {
	auto data = BytecodeInterpreterDataTestContainer{};
  auto moduleConfiguration = ska::lang::ModuleConfiguration<ska::bytecode::Interpreter> { data.storage.astCache, data.storage, *data.typeBuilder, *data.symbolsTypeUpdater, reservedKeywords, *data.parser, *data.interpreter};
  auto pathmodule = ska::lang::IOPathModule(moduleConfiguration);

  constexpr auto progStr =
  "var PathFcty = import \"bind:std.native.io.path\";"
  "var path = PathFcty.Build(\"" SKALANG_TEST_DIR "\");"
  "path.canonical();";

	auto script = Interpret(data, progStr);
	auto& gen = data.generator->generate(data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), data.storage);
	auto res = interpreted->variable(0);
	auto cellValue = res.nodeval<long>();
	CHECK(cellValue == 10);
}
