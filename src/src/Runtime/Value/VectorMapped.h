#pragma once
#include <unordered_map>
#include <iterator>
#include <vector>

namespace ska {

  template <template <class T> class Container, class Key, class Value>
  class container_mapped {
  public:
    container_mapped(Container<Value>&& origin) : m_container(std::forward<Container<Value>>(origin)) {}
    container_mapped() = default;
    ~container_mapped() = default;

    void emplace(Key&& key, Value&& value) {
      auto index = m_map.size();
      const auto [insertedIt, insertionHappened] = m_map.emplace(std::forward<Key>(key), index);
      index = std::distance(m_map.begin(), insertedIt);

      if (index >= m_container.size()) {
        if (index == m_container.size() + 1) {
          m_container.push_back(std::forward<Value>(value));
          return;
        } else {
          m_container.resize(index + 1);
        }
      }
      m_container[index] = std::forward<Value>(value);
    }

    Value& operator[](const Key& key) {
      const auto index = m_map.at(key);
      assert(index < m_container.size());
      return m_container[index];
    }

    const Value& operator[](const Key& key) const {
      const auto index = m_map.at(key);
      assert(index < m_container.size());
      return m_container[index];
    }

    const Container<Value>& underlying() const {
      return m_container;
    }

  private:
    std::unordered_map<Key, std::size_t> m_map;
    Container<Value> m_container;
  };

}