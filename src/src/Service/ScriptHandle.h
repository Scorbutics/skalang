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
		ScriptHandle(ScriptCache& cache, std::vector<Token> input, std::size_t startIndex = 0) :
			m_cache(cache),
			m_input(std::move(input), startIndex) {}
	
	public:
		SymbolTable& symbols() { return m_symbols; }
		const SymbolTable& symbols() const { return m_symbols; }
		
		MemoryTable& memory() { return m_memory; }
		const MemoryTable& memory() const { return m_memory; }

		ASTNode& rootNode() { assert(m_ast != nullptr);  return *m_ast; }
		const ASTNode& rootNode() const { assert(m_ast != nullptr);  return *m_ast; }

	private:
		friend class Script;
		
		ScriptCache& m_cache;
		TokenReader m_input;
		SymbolTable m_symbols;
		MemoryTable m_memory;
		ASTNodePtr m_ast;
		bool m_bridged = false;
	};

	
}