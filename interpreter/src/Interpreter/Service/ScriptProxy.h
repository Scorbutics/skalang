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

	using ScriptBridge = ska::ScriptBinding<ska::Script, ska::ScriptCache>;

	class ScriptProxy {
	public:
		ScriptProxy(ScriptBridge& binding);

		virtual ~ScriptProxy() = default;

		BridgeMemory<MemoryTablePtr> createMemory(Type type) { return { m_script.createMemory(), std::move(type) }; }

		NodeValue callFunction(Interpreter& interpreter, std::string importName, std::string functionName, std::vector<ska::NodeValue> parametersValues);
		MemoryLValue accessMemory(std::string importName, std::string field);

	private:
		ScriptBridge& m_binding;
		Script m_script;
	};
}
