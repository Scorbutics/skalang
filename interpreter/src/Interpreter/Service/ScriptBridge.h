#pragma once
#include <functional>
#include <cassert>
#include <vector>
#include <unordered_map>

#include "Runtime/Service/ScriptBinding.h"
#include "Interpreter/ScriptCache.h"
#include "Interpreter/Value/Script.h"

namespace ska {
	class Interpreter;

	class ScriptBridge :
        public ScriptBinding {
	public:
		ScriptBridge(
			ScriptCache& cache,
			std::string scriptName,
			TypeBuilder& typeBuilder,
			SymbolTableUpdater& symbolTypeUpdater,
			const ReservedKeywordsPool& reserved);

		virtual ~ScriptBridge() = default;

		void buildFunctions();

		void import(StatementParser& parser, Interpreter& interpreter, std::vector<std::pair<std::string, std::string>> imports);

		MemoryTablePtr createMemory() { return m_script.createMemory(); }

		NodeValue callFunction(Interpreter& interpreter, std::string importName, std::string functionName, std::vector<ska::NodeValue> parametersValues);
		MemoryLValue accessMemory(std::string importName, std::string field);

	private:
		std::string m_name;
		Script m_script;
		ScriptCache& m_cache;
	};
}
