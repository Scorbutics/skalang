#include "Config/LoggerConfigLang.h"
#include "NodeValue/AST.h"
#include "BytecodeGenerationContext.h"
#include "Generator/Value/BytecodeScript.h"
#include "Interpreter/Value/Script.h"

ska::BytecodeCell ska::BytecodeGenerationContext::cellFromValue(BytecodeCommand commandToGenerate) {
	auto& value = pointer();
	return { commandToGenerate, value.type().value(), Token{ value.name(), value.tokenType(), value.positionInScript()} };
}

ska::BytecodeGenerationContext::BytecodeGenerationContext(BytecodeScript& script) :
	m_script(&script),
	m_pointer(&script.program().rootNode()) {
}

ska::BytecodeGenerationContext::BytecodeGenerationContext(BytecodeScript& script, const ASTNode& node) :
	m_script(&script),
	m_pointer(&node) {
}
