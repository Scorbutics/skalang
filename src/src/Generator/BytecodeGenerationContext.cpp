#include "Config/LoggerConfigLang.h"
#include "NodeValue/AST.h"
#include "BytecodeGenerationContext.h"
#include "Interpreter/Value/Script.h"

ska::BytecodeCell ska::BytecodeGenerationContext::cellFromValue(BytecodeCommand commandToGenerate) {
	auto& value = pointer();
	return { commandToGenerate, value.type().value(), Token{ value.name(), value.tokenType(), value.positionInScript()} };
}

ska::BytecodeGenerationContext::BytecodeGenerationContext(ScriptHandle& program) :
	m_program(&program),
	m_pointer(&m_program->rootNode()) {
}

ska::BytecodeGenerationContext::BytecodeGenerationContext(Script& program) :
	m_program(program.handle()),
	m_pointer(&m_program->rootNode()) {
}

ska::BytecodeGenerationContext::BytecodeGenerationContext(ScriptHandle& program, const ASTNode& node) :
	m_program(&program),
	m_pointer(&node) {
}
