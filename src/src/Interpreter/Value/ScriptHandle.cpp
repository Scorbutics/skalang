#include "Config/LoggerConfigLang.h"
#include "ScriptHandle.h"

ska::ScriptHandle::ScriptHandle(ScriptCache& cache, std::vector<Token> input, std::size_t startIndex) : 
	m_cache(cache),
	m_input(std::move(input), startIndex),
	m_memory(MemoryTable::create()),
	m_currentMemory(m_memory) {
}
