#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include "NodeValue/Token.h"
#include "NodeValue.h"

namespace ska {
	class MemoryTable {
		using ChildrenScopedMemoryTable = std::vector<std::shared_ptr<MemoryTable>>;

	public:
		MemoryTable(MemoryTable& parent) :
			m_parent(&parent) {
		}

		MemoryTable() = default;
		~MemoryTable() = default;

		[[nodiscard]]
		MemoryTable* parent();

		MemoryTable& createNested();
		MemoryTable* endNested();
		MemoryTable* popNested();
		NodeValue& put(std::string name, NodeValue value);
		void put(std::string name, std::size_t index, NodeValue value);

		NodeValue* operator[](const std::string& key) {
			return m_current->inMemoryFind(key);
		}

		const NodeValue* operator[](const std::string& key) const {
			return m_current->inMemoryFind(key);
		}

		MemoryTable* zone(const std::string& key) {
			return m_current->inMemoryZone(key);
		}

		MemoryTable& pointTo(MemoryTable& memoryTable) {
			auto& last = *m_current;
			m_current = &memoryTable;
			return last;
		}

		NodeValue* operator()(const std::string& key) {
			auto valueIt = m_current->m_memory.find(key);
			return valueIt == m_current->m_memory.end() ? nullptr : &(valueIt->second);
		}

		const NodeValue* operator()(const std::string& key) const {
			const auto valueIt = m_current->m_memory.find(key);
			return valueIt == m_current->m_memory.end() ? nullptr : &(valueIt->second);
		}

	private:
		const NodeValue* inMemoryFind(const std::string& key) const {
			const auto valueIt = m_memory.find(key);
			if (valueIt == m_memory.end()) {
				auto* backScope = m_parent;
				return backScope == nullptr || backScope == this ? nullptr : backScope->inMemoryFind(key);
			}
			return valueIt == m_memory.end() ? nullptr : &(valueIt->second);
		}

		NodeValue* inMemoryFind(const std::string& key) {
			const auto valueIt = m_memory.find(key);
			if (valueIt == m_memory.end()) {
				auto* backScope = m_parent;
				return backScope == nullptr || backScope == this ? nullptr : backScope->inMemoryFind(key);
			}
			return valueIt == m_memory.end() ? nullptr : &(valueIt->second);
		}

		MemoryTable* inMemoryZone(const std::string& key) {
			const auto valueIt = m_memory.find(key);
			if (valueIt == m_memory.end()) {
				return m_parent == nullptr || m_parent == this ? nullptr : m_parent->inMemoryZone(key);
			}
			return valueIt == m_memory.end() ? nullptr : this;
		}

		std::unordered_map<std::string, NodeValue> m_memory;
		ChildrenScopedMemoryTable m_children;
		MemoryTable* const m_parent = nullptr;
		MemoryTable* m_current = this;
	};
}
