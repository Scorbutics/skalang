#pragma once
#include <cassert>
#include <vector>
#include <algorithm>
#include <memory>
#include <unordered_map>

#include "NodeValue.h"
#include "Interpreter/MemoryTablePtr.h"

namespace ska {

	class MemoryTable : public  std::enable_shared_from_this<MemoryTable> {
		using MemoryLValue = std::pair<NodeValue*, MemoryTablePtr>;
		using MemoryCLValue = std::pair<const NodeValue*, MemoryTableCPtr>;

	public:
		MemoryTable(const MemoryTable&) = delete;
		MemoryTable& operator=(const MemoryTable&) = delete;

		~MemoryTable() = default;

		[[nodiscard]]
		std::weak_ptr<ska::MemoryTable> parent();

		[[nodiscard]]
		MemoryTable& down() {
			assert(!m_children.empty());
			return *m_children[0].get();
		}

		MemoryTablePtr& pushNested();
		void endNested();
		MemoryTable* popNested();
		MemoryLValue put(const std::string& name, NodeValue value);
		MemoryLValue emplace(const std::string& name, NodeValue value);
		void put(const std::string& name, std::size_t index, NodeValue value);

		MemoryLValue operator[](const std::string& key) {
			return inMemoryFind(key);
		}

		MemoryCLValue operator[](const std::string& key) const {
			return inMemoryFind(key);
		}


		MemoryLValue operator()(const std::string& key) {
			auto valueIt = m_memory.find(key);
			return valueIt == m_memory.end() ? std::make_pair(nullptr, nullptr) : std::make_pair(&(valueIt->second), shared_from_this());
		}

		MemoryCLValue operator()(const std::string& key) const {
			const auto valueIt = m_memory.find(key);
			return valueIt == m_memory.end() ? std::make_pair(nullptr, nullptr) : std::make_pair(&(valueIt->second), shared_from_this());
		}

		void clear() {
			m_children.clear();
			m_memory.clear();
		}

		static MemoryTablePtr create(MemoryTablePtr parent = nullptr) {
			return std::shared_ptr<MemoryTable>(new MemoryTable(parent));
		}

	private:
		MemoryTable(MemoryTable&& o) noexcept = default;
		MemoryTable& operator=(MemoryTable&& o) noexcept = default;

		MemoryTable(MemoryTablePtr parent) :
			m_parent(parent) {
		}

		MemoryCLValue inMemoryFind(const std::string& key) const {
			const auto valueIt = m_memory.find(key);
			if (valueIt == m_memory.end()) {
				auto& backScope = m_parent.lock();
				return backScope == nullptr || backScope == this ? std::make_pair(nullptr, nullptr) : backScope->inMemoryFind(key);
			}
			return valueIt == m_memory.end() ? std::make_pair(nullptr, nullptr) : std::make_pair(&(valueIt->second), shared_from_this());
		}

		MemoryLValue inMemoryFind(const std::string& key) {
			const auto valueIt = m_memory.find(key);
			if (valueIt == m_memory.end()) {
				auto& backScope = m_parent.lock();
				return backScope == nullptr || backScope == this ? std::make_pair(nullptr, nullptr) : backScope->inMemoryFind(key);
			}
			return valueIt == m_memory.end() ? std::make_pair(nullptr, nullptr) : std::make_pair(&(valueIt->second), shared_from_this());
		}

	private:
		using ChildrenScopedMemoryTable = std::vector<MemoryTablePtr>;

		std::unordered_map<std::string, NodeValue> m_memory;
		ChildrenScopedMemoryTable m_children;
		std::weak_ptr<MemoryTable> m_parent;
	};


}
