#include "MemoryTable.h"

ska::MemoryTable& ska::MemoryTable::parent() {
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

ska::MemoryTable& ska::MemoryTable::endNested() {
	m_current = &m_current->parent();
	return *m_current;
}

ska::NodeValue& ska::MemoryTable::put(std::string name, NodeValue value) {
	assert(!name.empty());
	auto* memValue = operator[](name);
	if (memValue != nullptr) {
		*memValue = std::move(value);
	} else {
		m_current->m_memory.emplace(name, std::move(value));
		memValue = &m_current->m_memory.at(name);
	}

	return *memValue;
}

void ska::MemoryTable::put(std::string name, std::size_t index, NodeValue value) {
	assert(!name.empty());
	auto* memValue = operator[](name);
	assert(memValue != nullptr);
	auto& arrayPtr = std::get<std::shared_ptr<std::vector<Token::Variant>>>(*memValue);
	(*arrayPtr)[index] = std::move(std::get<Token::Variant>(value));
}
