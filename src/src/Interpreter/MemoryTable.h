#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include "NodeValue/Token.h"
#include "NodeValue.h"

namespace ska {
	class MemoryTable {
		using ChildrenScopedMemoryTable = std::vector<std::shared_ptr<MemoryTable>>;
		using MemoryLValue = std::pair<NodeValue*, MemoryTable*>;
		using MemoryCLValue = std::pair<const NodeValue*, const MemoryTable*>;
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
		MemoryLValue put(std::string name, NodeValue value);
		void put(std::string name, std::size_t index, NodeValue value);

		MemoryLValue operator[](const std::string& key) {
			return m_current->inMemoryFind(key);
		}

		MemoryCLValue operator[](const std::string& key) const {
			return m_current->inMemoryFind(key);
		}

		MemoryTable& pointTo(MemoryTable& memoryTable) {
			auto& last = *m_current;
			m_current = &memoryTable;
			return last;
		}

		MemoryLValue operator()(const std::string& key) {
			auto valueIt = m_current->m_memory.find(key);
			return valueIt == m_current->m_memory.end() ? std::make_pair(nullptr, nullptr) : std::make_pair(&(valueIt->second), this);
		}

		MemoryCLValue operator()(const std::string& key) const {
			const auto valueIt = m_current->m_memory.find(key);
			return valueIt == m_current->m_memory.end() ? std::make_pair(nullptr, nullptr) : std::make_pair(&(valueIt->second), this);
		}

	private:
		MemoryCLValue inMemoryFind(const std::string& key) const {
			const auto valueIt = m_memory.find(key);
			if (valueIt == m_memory.end()) {
				auto* backScope = m_parent;
				return backScope == nullptr || backScope == this ? std::make_pair(nullptr, nullptr) : backScope->inMemoryFind(key);
			}
			return valueIt == m_memory.end() ? std::make_pair(nullptr, nullptr) : std::make_pair(&(valueIt->second), this);
		}

		MemoryLValue inMemoryFind(const std::string& key) {
			const auto valueIt = m_memory.find(key);
			if (valueIt == m_memory.end()) {
				auto* backScope = m_parent;
				return backScope == nullptr || backScope == this ? std::make_pair(nullptr, nullptr) : backScope->inMemoryFind(key);
			}
			return valueIt == m_memory.end() ? std::make_pair(nullptr, nullptr) : std::make_pair(&(valueIt->second), this);
		}

		std::unordered_map<std::string, NodeValue> m_memory;
		ChildrenScopedMemoryTable m_children;
		MemoryTable* const m_parent = nullptr;
		MemoryTable* m_current = this;
	};
}
