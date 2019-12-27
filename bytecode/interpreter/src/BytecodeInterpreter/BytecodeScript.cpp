#include "Config/LoggerConfigLang.h"
#include "BytecodeScript.h"

#include "BytecodeInterpreter/BytecodeInterpreter.h"
#include "Generator/BytecodeGenerator.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::bytecode::Script);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::Script)

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

void ska::bytecode::Script::fromBridge(ASTNodePtr astRoot, Interpreter& interpreter) {
  //LOG_DEBUG << "%14cGenerating " << bindings.size() << " bindings for script " << m_serviceGen.name();

  m_serviceGen.fromBridge(std::move(astRoot));

  /*
  auto templateScriptGenerationContext = GenerationContext{m_cache, templateScriptAst};

  //TODO parameter : "Fcty"
  const auto* symbol = templateScriptAst.symbols()["Fcty"];
  //TODO exceptions instead of assertions
  assert(symbol != nullptr && !symbol->getType().compound().empty());
  const auto* subSymbol = symbol->getType().compound().back().symbol();
  assert(subSymbol != nullptr);

  auto symbolInfo = ska::bytecode::SymbolInfo{1, subSymbol->getName(), m_serviceGen.id() };
  symbolInfo.binding = true;
  m_cache.setSymbolInfo(*subSymbol, std::move(symbolInfo)),

  LOG_DEBUG << " %14cConstructor sub-symbol type to bind : \"" << (*subSymbol).getType() << "\"";

  m_serviceGen.generate(m_cache, interpreter.generator());
  */

  execute(interpreter);
}

std::unique_ptr<ska::bytecode::Executor> ska::bytecode::Script::execute(Interpreter& interpreter) {
  return interpreter.interpret(m_serviceGen.id(), m_cache);
}
