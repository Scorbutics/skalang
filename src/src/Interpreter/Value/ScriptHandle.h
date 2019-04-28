#pragma once
#include <vector>
#include <memory>
#include "NodeValue/ASTNodePtr.h"
#include "Service/SymbolTable.h"
#include "Service/TokenReader.h"
#include "NodeValue/Token.h"

namespace ska {
	class Script;
	struct ScriptHandle;
	using ScriptHandlePtr = std::unique_ptr<ScriptHandle>;

	using ScriptCache = std::unordered_map<std::string, ScriptHandlePtr>;

	struct ScriptHandle {
		ScriptHandle(ScriptCache& cache, std::vector<Token> input, std::size_t startIndex = 0);
	
	public:
		SymbolTable& symbols() { return m_symbols; }
		const SymbolTable& symbols() const { return m_symbols; }
		
		MemoryTablePtr& currentMemory() { return m_currentMemory; }
		const MemoryTablePtr& currentMemory() const { return m_currentMemory; }

		MemoryTable& downMemory() { assert(m_currentMemory != nullptr);  return m_currentMemory->down(); }
		const MemoryTable& downMemory() const { assert(m_currentMemory != nullptr);  return m_currentMemory->down(); }

		ASTNode& rootNode() { assert(m_ast != nullptr);  return *m_ast; }
		const ASTNode& rootNode() const { assert(m_ast != nullptr);  return *m_ast; }

	private:
		friend class Script;
		
		ScriptCache& m_cache;
		TokenReader m_input;
		SymbolTable m_symbols;
		MemoryTablePtr m_memory;
		MemoryTablePtr m_currentMemory;
		ASTNodePtr m_ast;
		bool m_bridged = false;
	};

	
}