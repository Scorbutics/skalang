#include "Config/LoggerConfigLang.h"
#include "NodeValue/AST.h"
#include "BytecodeGenerationContext.h"
#include "Generator/Value/BytecodeScript.h"
#include "NodeValue/ScriptAST.h"

ska::bytecode::GenerationContext::GenerationContext(ScriptGeneration& script) :
	m_script(&script),
	m_pointer(&script.program().rootNode()) {
}

ska::bytecode::GenerationContext::GenerationContext(ScriptGeneration& script, const ASTNode& node, std::size_t scopeLevel) :
	m_script(&script),
	m_pointer(&node),
	m_scopeLevel(scopeLevel) {
}
