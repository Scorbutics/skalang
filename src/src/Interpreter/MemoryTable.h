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
			m_parent(parent) {
		}

		MemoryTable() = default;
		~MemoryTable() = default;

		[[nodiscard]]
		MemoryTable& parent();

		MemoryTable& createNested();
		MemoryTable& endNested();
		NodeValue& put(std::string name, NodeValue value);

		NodeValue* operator[](const std::string& key) {
			auto valueIt = m_current->m_memory.find(key);
			if (valueIt == m_current->m_memory.end()) {
				return &m_current->m_parent == this ? nullptr : m_current->m_parent[key];
			}
			return valueIt == m_current->m_memory.end() ? nullptr : &(valueIt->second);
		}

		const NodeValue* operator[](const std::string& key) const {
			const auto valueIt = m_current->m_memory.find(key);
			if (valueIt == m_current->m_memory.end()) {
				return &m_current->m_parent == this ? nullptr : m_current->m_parent[key];
			}
			return valueIt == m_current->m_memory.end() ? nullptr : &(valueIt->second);
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
		std::unordered_map<std::string, NodeValue> m_memory;
		ChildrenScopedMemoryTable m_children;
		MemoryTable& m_parent = *this;
		MemoryTable* m_current = this;
	};
}
