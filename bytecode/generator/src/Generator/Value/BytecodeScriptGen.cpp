#include "Config/LoggerConfigLang.h"
#include "BytecodeScriptGen.h"
#include "Runtime/Value/NodeValue.h"
#include "Generator/BytecodeGenerator.h"

ska::bytecode::ScriptGenerationService& ska::bytecode::ScriptGen::AddScript(ska::bytecode::ScriptCache& cache, std::vector<ska::Token> tokens, const std::string& name) {
	auto ast = ska::ScriptAST { cache.astCache, name, std::move(tokens)};
  if (cache.find(name) == cache.end()) {
		auto holder = ska::bytecode::ScriptGenerationService{ cache.id(name), ast };
    //auto handle = ska::bytecode::ScriptGen{ cache, std::move(holder) };
		cache.emplace(name, { std::move(holder) });
	}
	return cache.at(name).generation;
}

std::optional<ska::bytecode::Value> ska::bytecode::ScriptGen::getSymbol(const Symbol& symbol) const {
  return m_service.getSymbol(symbol);
}

ska::bytecode::ScriptGen::ScriptGen(ScriptCache& scriptCache, std::size_t scriptIndex) :
  m_cache(scriptCache), m_service(m_cache.at(scriptIndex).generation) {
  m_id = scriptIndex;
  m_ast = m_service.program();
}

ska::bytecode::ScriptGen::ScriptGen(ScriptCache& scriptCache, const std::string& fullName, std::vector<Token> tokens) :
  m_cache(scriptCache), m_service(AddScript(scriptCache, std::move(tokens), fullName)) {
  m_id = m_cache.id(m_service.program().name());
  m_ast = m_service.program();
}

ska::bytecode::ScriptGen::ScriptGen(ScriptCache& scriptCache, ScriptAST& scriptAST, const std::string& fullName) :
  m_cache(scriptCache), m_service(scriptCache.at(fullName).generation) {
  m_id = m_cache.id(m_service.program().name());
  m_ast = m_service.program();
}

ska::ScriptAST& ska::bytecode::ScriptGen::astScript() {
  return m_ast.value();
}

const ska::ScriptAST& ska::bytecode::ScriptGen::astScript() const {
  return m_ast.value();
}

void ska::bytecode::ScriptGen::generate(Generator& generator) {
  if (m_generated) {
    return;
  }

  //TODO simplifier tout ça
  auto genContext = GenerationContext { m_cache.output, m_id };
  m_cache.output.setOut(m_id, generator.generatePart(genContext));

  m_generated = true;
}
