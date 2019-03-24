#pragma once
#include <memory>
#include <unordered_map>
#include "ScriptPtr.h"
#include "ScriptHandle.h"
#include "ScriptCache.h"
#include "Interpreter/BridgeFunction.h"

namespace ska {
	class StatementParser;

    class Script {
	public:
		Script(ScriptHandle& handle) :
			m_cache(handle.m_cache) {
			m_handle = &handle;
		}

		Script(ScriptCache& scriptCache, const std::string& name, std::vector<Token> input, std::size_t startIndex = 0) :
			m_cache(scriptCache) {
            auto handle = std::make_unique<ScriptHandle>(m_cache, std::move(input), startIndex);
            if(m_cache.find(name) == m_cache.end()) {
				m_cache.emplace(name, std::move(handle));
            } else {
				m_inCache = true;
			}
            m_handle = m_cache.at(name).get();
		}
		virtual ~Script() = default;
	
		bool existsInCache(const std::string& name) const {
			return m_cache.find(name) != m_cache.end();
		}

		void rewind() {
			return m_handle->m_input.rewind();
		}

        const Token& readPrevious(std::size_t offset) const;
		bool canReadPrevious(std::size_t offset) const;
		bool empty() const;
        Token actual() const;
        const Token& match(const Token& t);
		const Token& match(const TokenType& t);
        bool expect(const Token& t);
		bool expect(const TokenType& type) const;

		void parse(StatementParser& parser, bool listen = true);

		ASTNodePtr statement(StatementParser& parser);
        ASTNodePtr optstatement(StatementParser& parser, const Token& mustNotBe = Token{});

		ASTNodePtr expr(StatementParser& parser);
		ASTNodePtr optexpr(StatementParser& parser, const Token& mustNotBe = Token{});

		ScriptPtr subParse(StatementParser& parser, const std::string& name, std::ifstream& file);
		ScriptPtr subScript(const std::string& name);

		ASTNode& fromBridge(std::vector<BridgeFunctionPtr> bindings);

		const auto& handle() const { return m_handle; }

		SymbolTable& symbols() { return m_handle->m_symbols; }
		const SymbolTable& symbols() const { return m_handle->m_symbols; }

		MemoryTable& memory() { return m_handle->m_memory; }
		const MemoryTable& memory() const { return m_handle->m_memory; }

		auto& rootNode() {
			return *m_handle->m_ast;
		}

		const auto& rootNode() const {
			return *m_handle->m_ast;
		}

		bool isBridged() const { return m_handle->m_bridged; }

	private:
        ScriptHandle* m_handle = nullptr;
		ScriptCache& m_cache;
		bool m_inCache = false;

	};

}
