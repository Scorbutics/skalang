#pragma once
#include <cassert>

namespace ska {
	class Script;
	class ASTNode;
	class NodeValue;
	class MemoryTable;

	class ExecutionContext {
	public:
		ExecutionContext() = default;
		ExecutionContext(Script& program);
		ExecutionContext(Script& program, ASTNode& node, NodeValue* value = nullptr, MemoryTable* table = nullptr) :
			m_program(&program),
			m_pointer(&node),
			m_value(value),
			m_table(table){
		}

		ASTNode& pointer() { assert(m_pointer != nullptr); return *m_pointer; }
		Script& program() { assert(m_program != nullptr); return *m_program; }

		NodeValue* value() { return m_value; }
		MemoryTable* table() { return m_table; }
	private:
		Script* m_program{};
		ASTNode* m_pointer{};
		
		NodeValue* m_value{};
		MemoryTable* m_table{};
	};
}