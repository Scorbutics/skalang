#include "MemoryTable.h"

ska::MemoryTable* ska::MemoryTable::parent() {
	return m_current->m_parent;
}

ska::MemoryTable & ska::MemoryTable::createNested() {
	m_current->m_children.push_back(std::make_shared<MemoryTable>(*m_current));
	auto& lastChild = *m_current->m_children.back();
	//No bad memory access possible when smart ptr are moved, that's why it's safe to return the address of contained item
	//even if we move the vector or if the vector moves its content automatically 
	m_current = &lastChild;
	return lastChild;
}

ska::MemoryTable* ska::MemoryTable::endNested() {
	//TODO : � remettre, pour le moment bloque le syst�me d'import de script
	//m_current->m_memory.clear();
	m_current = m_current->parent();
	return m_current;
}

ska::MemoryTable* ska::MemoryTable::popNested() {
	m_current = m_current->parent();
	if (m_current != nullptr) {
		m_current->m_children.pop_back();
	}
	return m_current;
}

ska::MemoryTable::MemoryLValue ska::MemoryTable::put(const std::string& name, NodeValue value) {
	assert(!name.empty());
	auto memValueZone = operator[](name);
	auto* memValue = memValueZone.first;
	if (memValue != nullptr) {
		*memValue = std::move(value);
	} else {
		m_current->m_memory.emplace(name, std::move(value));
		memValue = &m_current->m_memory.at(name);
	}

	return std::make_pair(memValue, m_current);
}

void ska::MemoryTable::put(const std::string& name, std::size_t index, NodeValue value) {
	assert(!name.empty());
	auto memValueZone = operator[](name);
	auto memValue = memValueZone.first;
	assert(memValue != nullptr);
	auto& arrayPtr = memValue->as<NodeValueArray>();
	(*arrayPtr)[index] = std::move(value.as<Token::Variant>());
}

ska::MemoryTable::MemoryLValue ska::MemoryTable::emplace(const std::string& name, NodeValue value) {
	assert(!name.empty());
	auto memValueZone = operator()(name);
	auto* memValue = memValueZone.first;
	if (memValue != nullptr) {
		*memValue = std::move(value);
	} else {
		m_current->m_memory.emplace(name, std::move(value));
		memValue = &m_current->m_memory.at(name);
	}

	return std::make_pair(memValue, m_current);
}