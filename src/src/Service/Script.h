#pragma once
#include <memory>
#include <unordered_map>
#include "ScriptPtr.h"
#include "NodeValue/ASTNodePtr.h"
#include "Service/SymbolTable.h"
#include "Service/TokenReader.h"
#include "NodeValue/Token.h"

namespace ska {
	class StatementParser;
	struct ScriptHandle {
            ScriptHandle(std::vector<Token> input, std::size_t startIndex = 0) : 
                m_input(std::move(input), startIndex) {}

        TokenReader m_input;
        SymbolTable m_symbols;
		MemoryTable m_memory;
        ASTNodePtr m_ast;
    };

    using ScriptHandlePtr = std::unique_ptr<ScriptHandle>;

    class Script {
	public:
		Script(std::unordered_map<std::string, ScriptHandlePtr>& scriptCache, const std::string& name, std::vector<Token> input, std::size_t startIndex = 0) :
			m_cache(scriptCache) {
            auto handle = std::make_unique<ScriptHandle>(std::move(input), startIndex);
            if(m_cache.find(name) == m_cache.end()) {
				m_cache.emplace(name, std::move(handle));
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

		SymbolTable& symbols() { return m_handle->m_symbols; }
		const SymbolTable& symbols() const { return m_handle->m_symbols; }

		MemoryTable& memory() { return m_handle->m_memory; }
		const MemoryTable& memory() const { return m_handle->m_memory; }

		auto& rootNode() {
			return *m_handle->m_ast;
		}

	private:
        ScriptHandle* m_handle = nullptr;
        std::unordered_map<std::string, ScriptHandlePtr>& m_cache;

	};

}
