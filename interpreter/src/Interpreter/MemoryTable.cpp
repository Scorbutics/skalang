#include "MemoryTable.h"

std::weak_ptr<ska::MemoryTable> ska::MemoryTable::parent() {
	return m_parent;
}

ska::MemoryTablePtr& ska::MemoryTable::internalPushNested() {
	m_children.push_back(MemoryTable::create(shared_from_this()));
	auto& lastChild = m_children.back();
	//No bad memory access possible when smart ptr are moved, that's why it's safe to return the address of contained item
	//even if we move the vector or if the vector moves its content automatically 
	return lastChild;
}

void ska::MemoryTable::internalEndNested() {
	//TODO : devrait être systématiquement un "popNested" sauf pour le cas d'un block type "root script" ?
	
	/*
	if(!isOrphan()) {
		popNested();
	}
	*/
}

void ska::MemoryTable::internalPopNested() {
	m_children.pop_back();
}

ska::MemoryLValue ska::MemoryTable::put(const std::string& name, NodeValue value) {
	assert(!name.empty());
	auto memValueZone = operator[](name);
	auto* memValue = memValueZone.first;
	if (memValue != nullptr) {
		*memValue = std::move(value);
	} else {
		m_memory.emplace(name, std::move(value));
		memValue = &m_memory.at(name);
	}

	return std::make_pair(memValue, shared_from_this());
}

void ska::MemoryTable::put(const std::string& name, std::size_t index, NodeValue value) {
	assert(!name.empty());
	auto memValueZone = operator[](name);
	auto memValue = memValueZone.first;
	assert(memValue != nullptr);
	auto& arrayPtr = memValue->as<NodeValueArray>();
	auto rvalue = std::make_pair(std::move(value), nullptr);
	(*arrayPtr)[index] = std::move(rvalue.first);
}

ska::MemoryLValue ska::MemoryTable::emplace(const std::string& name, NodeValue value) {
	assert(!name.empty());
	auto memValueZone = operator()(name);
	auto* memValue = memValueZone.first;
	if (memValue != nullptr) {
		*memValue = std::move(value);
	} else {
		m_memory.emplace(name, std::move(value));
		memValue = &m_memory.at(name);
	}

	return std::make_pair(memValue, shared_from_this());
}

ska::MemoryTableLock ska::MemoryTable::pushNested(bool pop, MemoryTablePtr* target) {
	return MemoryTableLock{*this, target, pop};
}


ska::MemoryTableLock::MemoryTableLock(MemoryTable& instance, MemoryTablePtr* cursorCurrent, bool pop) : 
	m_pop(pop),
	m_instance(instance),
	m_cursorCurrent(cursorCurrent) {
	if(m_cursorCurrent != nullptr) {
		*m_cursorCurrent = m_instance.internalPushNested();
	}
}

ska::MemoryTableLock::MemoryTableLock(MemoryTableLock&& m) noexcept :
	m_pop(m.m_pop),
	m_instance(m.m_instance),
	m_cursorCurrent(m.m_cursorCurrent),
	m_freed(m.m_freed) {
	m.m_freed = true;
}

void ska::MemoryTableLock::release() {
	if(m_freed) {
		return;
	}

	if(m_cursorCurrent != nullptr && *m_cursorCurrent != nullptr && (*m_cursorCurrent)->parent().lock() != nullptr) {
		*m_cursorCurrent = (*m_cursorCurrent)->parent().lock();
	}

	if(m_pop) {
		m_instance.internalPopNested();
	} else {
		m_instance.internalEndNested();
	}
	m_freed = true;
}

ska::MemoryTableLock::~MemoryTableLock() {
	release();
}
