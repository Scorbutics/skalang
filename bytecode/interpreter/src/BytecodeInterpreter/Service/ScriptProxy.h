#pragma once
#include <functional>
#include <cassert>
#include <vector>
#include <unordered_map>

#include "Runtime/Service/ScriptBinding.h"
#include "Generator/Value/BytecodeScript.h"

namespace ska {
	namespace bytecode {
		class Interpreter;

		using ScriptBridge = ska::ScriptBinding<ScriptGenerationService, ScriptCache>;

		class ScriptProxy {
		public:
			ScriptProxy(ScriptBridge& binding);

			virtual ~ScriptProxy() = default;

			/*
			TODO !

			BridgeMemory<MemoryTablePtr> createMemory(const BridgeImport& import) { return { m_script.createMemory(), import.symbols() }; }
			BridgeMemory<MemoryTablePtr> createMemory(const Symbol& symbol) { return { m_script.createMemory(), symbol }; }

			NodeValue callFunction(Interpreter& interpreter, std::string importName, std::string functionName, std::vector<ska::NodeValue> parametersValues);
			MemoryLValue accessMemory(std::string importName, std::string field);
			*/

		private:
			ScriptBridge& m_binding;
			ScriptGenerationService& m_script;
		};
	}
}
