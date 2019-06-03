#include "Config/LoggerConfigLang.h"
#include "ScriptHandle.h"

ska::ScriptHandle::ScriptHandle(ScriptCache& cache, std::vector<Token> input, std::size_t startIndex, std::string name) : 
	m_cache(cache),
	m_input(std::move(input), startIndex),
	m_memory(MemoryTable::create()),
	m_currentMemory(m_memory),
	m_name(std::move(name)) {
}
