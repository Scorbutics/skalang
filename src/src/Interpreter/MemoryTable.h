#pragma once
#include <cassert>
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
			m_parent(parent.m_current) {
		}

		MemoryTable() = default;
		~MemoryTable() = default;

		[[nodiscard]]
		MemoryTable* parent();
		MemoryTable& down() {
			assert(!m_current->m_children.empty());
			return *m_current->m_children[0].get();
		}


		MemoryTable& createNested();
		MemoryTable* endNested();
		MemoryTable* popNested();
		MemoryLValue put(const std::string& name, NodeValue value);
		MemoryLValue emplace(const std::string& name, NodeValue value);
		void put(const std::string& name, std::size_t index, NodeValue value);

		MemoryLValue operator[](const std::string& key) {
			return m_current->inMemoryFind(key);
		}

		MemoryCLValue operator[](const std::string& key) const {
			return m_current->inMemoryFind(key);
		}

		MemoryTable& pointTo(MemoryTable& memoryTable) {
			auto& last = *m_current;
			m_current = memoryTable.m_current;
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

		void clear() {
			m_children.clear();
			m_memory.clear();
			m_current = this;
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
