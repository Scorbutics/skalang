#include "Config/LoggerConfigLang.h"
#include "ScriptHandle.h"
#include "Interpreter/ScriptCache.h"

ska::ScriptHandle::ScriptHandle(ScriptCache& cache, ScriptHandleAST& astHandle) :
	m_cache(cache),
	m_memory(MemoryTable::create()),
	m_currentMemory(m_memory),
	m_handleAst(astHandle) {
}
