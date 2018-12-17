#include "MemoryTable.h"

ska::MemoryTable& ska::MemoryTable::parent() {
	return m_current->m_parent;
}

ska::MemoryTable & ska::MemoryTable::createNested() {
	m_current->m_children.push_back(std::make_unique<MemoryTable>(*m_current));
	auto& lastChild = *m_current->m_children.back();
	//No bad memory access possible when unique_ptr are moved, that's why it's safe to return the address of contained item
	//even if we move the vector or if the vector moves its content automatically 
	m_current = &lastChild;
	return lastChild;
}

ska::MemoryTable& ska::MemoryTable::endNested() {
	m_current = &m_current->parent();
	return *m_current;
}

ska::Token::Variant& ska::MemoryTable::put(std::string name, Token::Variant value) {
	
	if (m_current->m_memory.find(name) == m_current->m_memory.end()) {
		m_current->m_memory.emplace(name, std::move(value));
	} else {
		m_current->m_memory.at(name) = std::move(value);
	}

	return m_current->m_memory.at(name);
}
