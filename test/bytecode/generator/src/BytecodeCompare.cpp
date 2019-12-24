#include <doctest.h>
#include "BytecodeCompare.h"
#include "Generator/Value/BytecodeScriptGeneration.h"

void BytecodeCompare(const ska::bytecode::ScriptGeneration& ScriptGeneration, std::vector<ska::bytecode::BytecodePart> expected) {
  auto index = std::size_t {0};
  CHECK(ScriptGeneration.size() == expected.size());
  for(const auto& r : ScriptGeneration) {
    const auto equality =
      index < expected.size() &&
      r.command() == expected[index].command &&
      r.dest().toString() == expected[index].dest &&
      r.left().toString() == expected[index].left &&
      r.right().toString() == expected[index].right;
    CHECK(equality);
    index++;
  }
}