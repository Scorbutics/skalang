#include "Config/LoggerConfigLang.h"
#include "NodeValue/AST.h"
#include "BytecodeGenerationContext.h"
#include "Generator/Value/BytecodeScript.h"
#include "NodeValue/ScriptAST.h"

ska::bytecode::GenerationContext::GenerationContext(Script& script) :
	m_script(&script),
	m_pointer(&script.program().rootNode()) {
}

ska::bytecode::GenerationContext::GenerationContext(Script& script, const ASTNode& node) :
	m_script(&script),
	m_pointer(&node) {
}
