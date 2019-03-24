#include "ExecutionContext.h"
#include "Service/Script.h"

ska::ExecutionContext::ExecutionContext(Script& program) :
	m_program(program.handle()),
	m_pointer(&m_program->rootNode()){
}

ska::ExecutionContext::ExecutionContext(Script& program, ASTNode& node) : 
	m_program(program.handle()),
	m_pointer(&node){
}

