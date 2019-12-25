#include "Config/LoggerConfigLang.h"
#include "BytecodeScript.h"

#include "BytecodeInterpreter/BytecodeInterpreter.h"
#include "Generator/BytecodeGenerator.h"

ska::bytecode::Operand ska::bytecode::Script::findBytecodeMemoryFromSymbol(const Symbol& symbol) const {
  auto bytecodeSymbol = m_serviceGen.getSymbol(symbol);
  if (bytecodeSymbol.has_value()) {
    throw std::runtime_error("unable to find generated symbol \"" + symbol.getName() + "\" in script \"" + astScript().name() + "\"");
  }
  return bytecodeSymbol.value();
}

const ska::Symbol& ska::bytecode::Script::findSymbolFromString(const std::string& key) const {
  const auto& symbols = astScript().symbols();
  auto* symbol = symbols[key];
  if (symbol == nullptr) {
    throw std::runtime_error("unable to find AST symbol \"" + key + "\" in root script \"" + astScript().name() + "\"");
  }
  return *symbol;
}

ska::bytecode::RuntimeMemory ska::bytecode::Script::memoryField(const std::string& symbol) {
  auto& symbolAst = findSymbolFromString(symbol);
  return RuntimeMemory { findBytecodeMemoryFromSymbol(symbolAst) };
}

void ska::bytecode::Script::memoryFromBridge(Interpreter& interpreter, std::vector<BridgeFunctionPtr> bindings) {
  //TODO generate bindings !
    m_serviceGen.generate(m_cache, interpreter.generator());
  execute(interpreter);
}

std::unique_ptr<ska::bytecode::Executor> ska::bytecode::Script::execute(Interpreter& interpreter) {
  return interpreter.interpret(m_serviceGen.id(), m_cache);
}
