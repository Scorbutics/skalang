#pragma once
#include <memory>
#include <unordered_map>
#include "NodeValue/ASTNodePtr.h"
#include "Service/SymbolTable.h"
#include "Service/TokenReader.h"
#include "NodeValue/Token.h"

namespace ska {
	class StatementParser;
	struct ScriptHandle {
        template <class TokenContainer>
            ScriptHandle(TokenContainer&& input, std::size_t startIndex = 0) : 
                m_input(std::forward<TokenContainer>(input), startIndex) {}

        TokenReader m_input;
        SymbolTable m_symbols;
        ASTNodePtr m_ast;
    };

    using ScriptHandlePtr = std::unique_ptr<ScriptHandle>;

    class Script {
	public:
        template<class TokenContainer>
		Script(const std::string& name, TokenContainer&& input, std::size_t startIndex = 0) {
            auto handle = std::make_unique<ScriptHandle>(std::forward<TokenContainer>(input), startIndex);
            if(map.find(name) == map.end()) {
                map.emplace(name, std::move(handle));
            }
            m_handle = map.at(name).get();
		}
		virtual ~Script() = default;
	
        const Token& readPrevious(std::size_t offset) const;
		bool canReadPrevious(std::size_t offset) const;
		bool empty() const;
        Token actual() const;
        const Token& match(const Token& t);
		const Token& match(const TokenType& t);
        bool expect(const Token& t);
		bool expect(const TokenType& type) const;

		ASTNodePtr parse(StatementParser& parser, bool listen = true);

		ASTNodePtr statement(StatementParser& parser);
        ASTNodePtr optstatement(StatementParser& parser, const Token& mustNotBe = Token{});

		ASTNodePtr expr(StatementParser& parser);
		ASTNodePtr optexpr(StatementParser& parser, const Token& mustNotBe = Token{});

		SymbolTable& symbols() {
			return m_handle->m_symbols;
		}

	private:
        ScriptHandle* m_handle = nullptr;
        static std::unordered_map<std::string, ScriptHandlePtr> map;
	};
}
