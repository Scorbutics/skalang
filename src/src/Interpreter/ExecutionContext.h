#pragma once
#include <cassert>

namespace ska {
	
	class ASTNode;
	class NodeValue;
	class MemoryTable;
	struct ScriptHandle;
	class Script;

	class ExecutionContext {
		friend bool operator!=(const ska::ExecutionContext& lhs, const ska::ExecutionContext& rhs);
	public:
		ExecutionContext() = default;
		explicit ExecutionContext(ScriptHandle& program);
		explicit ExecutionContext(Script& program);
		ExecutionContext(Script& program, ASTNode& node);

		ASTNode& pointer() { assert(m_pointer != nullptr); return *m_pointer; }
		ScriptHandle& program() { assert(m_program != nullptr); return *m_program; }

	private:
		ScriptHandle* m_program {};
		ASTNode* m_pointer{};
	};


	inline bool operator!=(const ska::ExecutionContext& lhs, const ska::ExecutionContext& rhs) {
		return lhs.m_program != rhs.m_program || lhs.m_pointer != lhs.m_pointer;
	}

	inline bool operator==(const ska::ExecutionContext& lhs, const ska::ExecutionContext& rhs) {
		return !operator!=(lhs, rhs);
	}

}