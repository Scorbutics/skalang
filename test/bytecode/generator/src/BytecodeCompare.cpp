#include <doctest.h>
#include "BytecodeCompare.h"
#include "Generator/Value/BytecodeScriptGeneration.h"

static std::string HandleScriptSuffix(std::size_t scriptId, std::string str) {
    if (str.empty() || str[0] != 'V' && str[0] != 'R') {
        return str;
    }
    const auto dotPos = str.find_last_of(':');
    if (dotPos == std::string::npos) {
        return str + ":" + std::to_string(scriptId);
    }
    return str;
}

void BytecodeCompare(const ska::bytecode::ScriptGeneration& scriptGeneration, std::vector<ska::bytecode::BytecodePart> expected) {
  auto index = std::size_t {0};
  CHECK(scriptGeneration.size() == expected.size());
  for(const auto& r : scriptGeneration) {
	const auto equality =
  	index < expected.size() &&
  	r.command() == expected[index].command &&
  	r.dest().toString() == HandleScriptSuffix(scriptGeneration.id(), expected[index].dest) &&
  	r.left().toString() == HandleScriptSuffix(scriptGeneration.id(), expected[index].left) &&
  	r.right().toString() == HandleScriptSuffix(scriptGeneration.id(), expected[index].right);
	CHECK(equality);
	index++;
  }
}
