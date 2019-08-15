#pragma once
#include <memory>
#include <unordered_map>
#include "ScriptPtr.h"
#include "ScriptHandle.h"
#include "Interpreter/ScriptCache.h"
#include "BridgeFunction.h"
#include "NodeValue/ScriptAST.h"
#include "Interpreter/Value/BridgeMemory.h"

namespace ska {
	class StatementParser;

    class Script {
	public:
		Script(ScriptHandle& handle) :
			m_cache(handle.m_cache),
			m_ast(handle.m_handleAst) {
			m_handle = &handle;
		}

		Script(ScriptCache& scriptCache, ScriptAST& scriptAST, const std::string& fullName);

		Script(ScriptCache& scriptCache, const std::string& name, std::vector<Token> input, std::size_t startIndex = 0);
		virtual ~Script() = default;
	
		bool existsInCache(const std::string& name) const {
			return m_cache.cache.find(name) != m_cache.cache.end();
		}

		ASTNode& fromBridge(std::vector<BridgeMemory> bindings);

		const auto& handle() const { return m_handle; }

		ScriptPtr useImport(const std::string& name);
		ScriptPtr createImport(const std::string& name);

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

		bool isBridged() const { return m_handle != nullptr && m_handle->m_bridged; }

		ScriptAST& astScript() { return m_ast; }

		const std::string& name() const { return m_ast.name(); }

		SymbolTable& symbols() { return m_ast.symbols(); }
		const SymbolTable& symbols() const { return m_ast.symbols(); }

	private:
		void memoryFromBridge(const ASTNode& declaredAstBlock, std::vector<BridgeMemory> bindings);
		static ScriptHandle* buildHandle(ScriptCache& cache, ScriptHandleAST& handleAST, const std::string& name, bool& inCache);

        ScriptHandle* m_handle = nullptr;
		ScriptCache& m_cache;
		bool m_inCache = false;

		ScriptAST m_ast;
	};

}
