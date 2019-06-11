#pragma once
#include <memory>
#include <unordered_map>
#include "ScriptPtr.h"
#include "ScriptHandle.h"
#include "Interpreter/ScriptCache.h"
#include "Interpreter/Value/BridgeFunction.h"

namespace ska {
	class StatementParser;

    class Script {
	public:
		Script(ScriptHandle& handle) :
			m_cache(handle.m_cache) {
			m_handle = &handle;
		}

		Script(ScriptCache& scriptCache, const std::string& name, std::vector<Token> input, std::size_t startIndex = 0);
		virtual ~Script() = default;
	
		bool existsInCache(const std::string& name) const {
			return m_cache.find(name) != m_cache.end();
		}

		void rewind() {
			return m_handle->m_input.rewind();
		}

		auto& reader() { return m_handle->m_input; }

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

		ASTNode& fromBridge(std::vector<BridgeMemory> bindings);

		const auto& handle() const { return m_handle; }

		SymbolTable& symbols() { return m_handle->m_symbols; }
		const SymbolTable& symbols() const { return m_handle->m_symbols; }

		const std::string& name() const { return m_handle->name(); }

		auto pushNestedMemory(bool pop) {
			auto lock = m_handle->m_currentMemory->pushNested(pop, &m_handle->m_currentMemory);
			return lock;
		}
		
		template <class T>
		auto putMemory(const std::string& s, T&& value) {
			return m_handle->m_currentMemory->put(s, std::forward<T>(value));
		}

		MemoryTablePtr pointMemoryTo(MemoryTablePtr& to) {
			auto actualMemory = m_handle->m_currentMemory;
			m_handle->m_currentMemory = to;
			return actualMemory;
		}

		MemoryTablePtr createMemory() {
			return MemoryTable::create(m_handle->m_currentMemory);
		}

		auto findInMemoryTree(const std::string& key) {
			return (*m_handle->m_currentMemory)[key];
		}

		auto findInMemory(const std::string& key) {
			return (*m_handle->m_currentMemory)(key);
		}

		template <class T>
		auto emplaceMemory(const std::string& key, T&& value) {
			return m_handle->m_currentMemory->emplace(key, std::forward<T>(value));
		}

		auto& rootNode() {
			return *m_handle->m_ast;
		}

		const auto& rootNode() const {
			return *m_handle->m_ast;
		}

		bool isBridged() const { return m_handle != nullptr && m_handle->m_bridged; }

	private:
        ScriptHandle* m_handle = nullptr;
		ScriptCache& m_cache;
		bool m_inCache = false;

	};

}
