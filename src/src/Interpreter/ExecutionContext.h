#pragma once
#include <cassert>

namespace ska {
	class Script;
	class ASTNode;
	class NodeValue;
	class MemoryTable;

	class ExecutionContext {
		friend bool operator!=(const ska::ExecutionContext& lhs, const ska::ExecutionContext& rhs);
	public:
		ExecutionContext() = default;
		explicit ExecutionContext(Script& program);
		ExecutionContext(Script& program, ASTNode& node) :
			m_program(&program),
			m_pointer(&node){
		}

		ASTNode& pointer() { assert(m_pointer != nullptr); return *m_pointer; }
		Script& program() { assert(m_program != nullptr); return *m_program; }

	private:
		Script* m_program{};
		ASTNode* m_pointer{};
	};


	inline bool operator!=(const ska::ExecutionContext& lhs, const ska::ExecutionContext& rhs) {
		return lhs.m_program != rhs.m_program || lhs.m_pointer != lhs.m_pointer;
	}

	inline bool operator==(const ska::ExecutionContext& lhs, const ska::ExecutionContext& rhs) {
		return !operator!=(lhs, rhs);
	}

}