#include "ExecutionContext.h"
#include "Interpreter/Value/Script.h"

ska::ExecutionContext::ExecutionContext(ScriptHandle& program) : 
	m_program(&program),
	m_pointer(&m_program->rootNode()),
	m_currentMemory(m_program->currentMemory()) {
}

ska::ExecutionContext::ExecutionContext(Script& program) :
	m_program(program.handle()),
	m_pointer(&m_program->rootNode()),
	m_currentMemory(program.handle()->currentMemory()) {
}

ska::ExecutionContext::ExecutionContext(Script& program, ASTNode& node) : 
	m_program(program.handle()),
	m_pointer(&node),
	m_currentMemory(program.handle()->currentMemory()) {
}

