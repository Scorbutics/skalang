#pragma once
#include <vector>
#include <memory>
#include "ASTNodePtr.h"
#include "Service/SymbolTable.h"
#include "Service/TokenReader.h"
#include "Token.h"


namespace ska {
	class ScriptAST;
	
	struct ScriptHandleAST;
	using ScriptHandleASTPtr = std::unique_ptr<ScriptHandleAST>;
	using ScriptCacheAST = std::unordered_map<std::string, ScriptHandleASTPtr>;

	struct ScriptHandleAST {
	
	public:
		ScriptHandleAST(const ScriptHandleAST&) = delete;
		ScriptHandleAST& operator=(const ScriptHandleAST&) = delete;

		SymbolTable& symbols() { return m_symbols; }
		const SymbolTable& symbols() const { return m_symbols; }
		
		ASTNode& rootNode() { assert(m_ast != nullptr); return *m_ast; }
		const ASTNode& rootNode() const { assert(m_ast != nullptr); return *m_ast; }

		const std::string& name() const { return m_name; }

	private:
		ScriptHandleAST(ScriptCacheAST& cache, std::vector<Token> input, std::size_t startIndex = 0, std::string name = "");
		friend class ScriptAST;
		
		ScriptCacheAST& m_cache;
		TokenReader m_input;
		SymbolTable m_symbols;
		ASTNodePtr m_ast;
		std::string m_name;
		bool m_bridged = false;
	};

	using ScriptHandleASTPtr = std::unique_ptr<ScriptHandleAST>;
}
