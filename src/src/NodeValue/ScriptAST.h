#pragma once
#include <memory>
#include <unordered_map>
#include "ScriptASTPtr.h"
#include "ScriptHandleAST.h"
#include "ScriptCacheAST.h"

namespace ska {
	class StatementParser;

    class ScriptAST {
	public:
		ScriptAST(ScriptHandleAST& handle) : 
			m_cache(&handle.m_cache) {
			m_handle = &handle;
		}

		ScriptAST(ScriptCacheAST& scriptCache, const std::string& name, std::vector<Token> input, std::size_t startIndex = 0);
		virtual ~ScriptAST() = default;
	
		bool existsInCache(const std::string& name) const {
			return m_cache->find(name) != m_cache->end();
		}

		void rewind() {
			return m_handle->m_input.rewind();
		}

		auto& reader() { return m_handle->m_input; }

		ASTNodePtr statement(StatementParser& parser);
		ASTNodePtr optstatement(StatementParser& parser, const Token& mustNotBe);
		ASTNodePtr expr(StatementParser& parser);
		ASTNodePtr optexpr(StatementParser& parser, const Token& mustNotBe);

		void parse(StatementParser& parser, bool listen = true);

		ScriptASTPtr subParse(StatementParser& parser, const std::string& name, std::ifstream& file);
		ScriptASTPtr useImport(const std::string& name);

		ASTNode& fromBridge(std::vector<ASTNodePtr> bindings);

		const auto& handle() const { return m_handle; }

		SymbolTable& symbols() { return m_handle->m_symbols; }
		const SymbolTable& symbols() const { return m_handle->m_symbols; }

		const std::string& name() const { return m_handle->name(); }

		auto& rootNode() {
			return *m_handle->m_ast;
		}

		const auto& rootNode() const {
			return *m_handle->m_ast;
		}

		bool isBridged() const { return m_handle != nullptr && m_handle->m_bridged; }

	private:
		void astFromBridge(std::vector<ASTNodePtr> bindings);

        ScriptHandleAST* m_handle = nullptr;
		ScriptCacheAST* m_cache;
		bool m_inCache = false;
	};

}
