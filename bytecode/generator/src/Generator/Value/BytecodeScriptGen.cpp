#include "BytecodeScriptGen.h"

static ska::bytecode::ScriptGenerationService AddScript(ska::bytecode::ScriptCache& cache, std::vector<ska::Token> tokens, const std::string& name) {
	auto ast = ska::ScriptAST { cache.astCache, name, std::move(tokens)};
  if (cache.find(name) == cache.end()) {
		auto script = ska::bytecode::ScriptGenerationService{ cache.id(name), ast };
    auto handle = ska::bytecode::ScriptGen{ std::move(script) };
		cache.emplace(name, std::move(handle));
	}
	return std::move(cache.at(name).service);
}

ska::bytecode::ScriptGen::ScriptGen(ScriptCache& scriptCache, const std::string& fullName, std::vector<Token> tokens) :
  service(AddScript(scriptCache, std::move(tokens), fullName)) {
}

ska::bytecode::ScriptGen::ScriptGen(ScriptCache& scriptCache, ScriptAST& scriptAST, const std::string& fullName) :
  service(scriptCache.id(fullName), scriptAST) {
}

ska::ScriptAST& ska::bytecode::ScriptGen::astScript() {
  if (!m_ast.has_value()) {
    m_ast = service.program();
  }
  return m_ast.value();
}

void ska::bytecode::ScriptGen::memoryFromBridge(std::vector<BridgeFunctionPtr> bindings) {
  //TODO
}