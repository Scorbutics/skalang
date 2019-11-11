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
	class ScriptCache;
	struct FunctionContext {
		ASTNode* node;
		MemoryTablePtr memory;
	};

	struct ScriptHandle {
	
	public:
		MemoryTablePtr& currentMemory() { return m_currentMemory; }
		const MemoryTablePtr& currentMemory() const { return m_currentMemory; }

		MemoryTable& downMemory() { assert(m_currentMemory != nullptr);  return m_currentMemory->down(); }
		const MemoryTable& downMemory() const { assert(m_currentMemory != nullptr);  return m_currentMemory->down(); }

		auto& rootNode() { return m_handleAst.rootNode(); }

		std::size_t pushFunction(ASTNode& function) {
			auto index = m_registeredFunctions.size();
			m_registeredFunctions.push_back({ &function, m_currentMemory });
			return index;
		}

		auto* getFunction(std::size_t functionId) {
			assert(m_registeredFunctions.size() > functionId);
			return &m_registeredFunctions[functionId];
		}

	private:
		ScriptHandle(ScriptCache& cache, ScriptHandleAST& astHandle);
		friend class Script;
		
		std::vector<FunctionContext> m_registeredFunctions;
		ScriptCache& m_cache;	
		MemoryTablePtr m_memory;
		MemoryTablePtr m_currentMemory;
		bool m_bridged = false;

		ScriptHandleAST& m_handleAst;
	};

	
}