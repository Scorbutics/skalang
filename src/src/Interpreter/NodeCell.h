#pragma once
#include "NodeXValue.h"

namespace ska {
	class NodeCell {
	public:
		NodeCell();
		NodeCell(NodeLValue lvalue);
		NodeCell(NodeRValue rvalue);

		NodeLValue asLvalue();
		NodeRValue asRvalue();

		NodeCell operator()(const std::string& key);

		NodeCell(NodeCell&&) noexcept = default;
		NodeCell& operator=(NodeCell&&) noexcept = default;

		NodeCell(const NodeCell&) = delete;
		NodeCell& operator=(const NodeCell&) = delete;

		template<class T> auto& as() { return std::get<T>(m_variant); }
		template<class T> const auto& as() const { return std::get<T>(m_variant); }
		
		friend bool operator==(const NodeCell& lhs, const NodeCell& rhs);
	
	private:
		bool isLvalue() const { return std::holds_alternative<NodeValue*>(m_variant); }

		NodeCell build(MemoryLValue& memoryField);

		std::variant<NodeValue, NodeValue*> m_variant;
		MemoryTablePtr m_memory;
	};

	inline bool operator==(const ska::NodeCell& lhs, const ska::NodeCell& rhs) {
		return lhs.m_variant == rhs.m_variant;
	}
}