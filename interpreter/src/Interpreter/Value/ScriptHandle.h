#pragma once
#include <vector>
#include <memory>
#include "NodeValue/ASTNodePtr.h"
#include "NodeValue/ScriptHandleAST.h"
#include "Service/SymbolTable.h"
#include "Service/TokenReader.h"
#include "NodeValue/Token.h"
#include "Interpreter/MemoryTable.h"

namespace ska {
	class Script;
	struct ScriptHandle;
	using ScriptHandlePtr = std::unique_ptr<ScriptHandle>;
	struct ScriptCache;
	struct ScriptHandle {
	
	public:
		MemoryTablePtr& currentMemory() { return m_currentMemory; }
		const MemoryTablePtr& currentMemory() const { return m_currentMemory; }

		MemoryTable& downMemory() { assert(m_currentMemory != nullptr);  return m_currentMemory->down(); }
		const MemoryTable& downMemory() const { assert(m_currentMemory != nullptr);  return m_currentMemory->down(); }

		auto& rootNode() { return m_handleAst.rootNode(); }

	private:
		ScriptHandle(ScriptCache& cache, ScriptHandleAST& astHandle);
		friend class Script;
		
		ScriptCache& m_cache;	
		MemoryTablePtr m_memory;
		MemoryTablePtr m_currentMemory;
		bool m_bridged = false;

		ScriptHandleAST& m_handleAst;
	};

	
}