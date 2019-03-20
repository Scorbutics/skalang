#include "ExecutionContext.h"
#include "Service/Script.h"

ska::ExecutionContext::ExecutionContext(Script& program) :
	m_program(&program),
	m_pointer(&program.rootNode()){
}
