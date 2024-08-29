#pragma once

#include <cassert>
#include <vector>
#include "NodeValue.h"

namespace ska {
	class NodeValue;

	class PlainMemoryTable {
	public:
		template <class T>
		const T& value(std::size_t index) const {
			return m_data[index].nodeval<T>();
		}

		template <class T>
		T& value(std::size_t index) {
			return m_data[index].nodeval<T>();
		}

		const NodeValue& operator[](std::size_t index) const {
			assert(m_data.size() > index);
			return m_data[index];
		}

		NodeValue* get_if(std::size_t index) {
			if (index < m_data.size()) {
				return &m_data[index];
			}
			return nullptr;
		}

		template <class T>
		void push(std::size_t index, T&& src) {
			if(index >= m_data.size()) {
				if(index == m_data.size()) {
					m_data.push_back(std::forward<T>(src));
					return;
				}
				m_data.resize(index + 1);
			}
			m_data[index] = std::forward<T>(src);
		}

		auto& back() { return m_data.back(); }
		const auto& back() const { return m_data.back(); }

		bool empty() const { return m_data.empty(); }
		auto size() const { return m_data.size(); }
	private:
		std::vector<NodeValue> m_data;
	};

}
