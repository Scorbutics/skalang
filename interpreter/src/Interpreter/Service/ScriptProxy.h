#pragma once
#include <functional>
#include <cassert>
#include <vector>
#include <unordered_map>

#include "Runtime/Service/ScriptBinding.h"
#include "Interpreter/ScriptCache.h"
#include "Interpreter/Value/Script.h"
#include "Runtime/Value/InterpreterTypes.h"
#include "Service/ScriptNameBuilder.h"

#include "Interpreter/InterpreterOperatorFunctionCall.h"

namespace ska {
	
	template <class Interpreter>
	using ScriptBridge = ska::ScriptBinding<
		typename InterpreterTypes<Interpreter>::Script,
		typename InterpreterTypes<Interpreter>::ScriptCache
	>;

	class ScriptProxyAST {
		//TODO static & bytecode gen part of ScriptProxy here
	};

	template <class Interpreter>
	class ScriptProxy {
		using Memory = typename InterpreterTypes<Interpreter>::Memory;
	public:
		ScriptProxy(ScriptBridge<Interpreter>& binding) : m_binding(binding),
			m_script(binding.script()) {
			SLOG(LogLevel::Info) << "Creating script proxy : " << m_script.name();
		}

		virtual ~ScriptProxy() = default;

		BridgeMemory<Memory> createMemory(const BridgeImport& import) { return { m_script.createMemory(), import.symbols() }; }
		BridgeMemory<Memory> createMemory(const Symbol& symbol) { return { m_script.createMemory(), symbol }; }

		NodeValue callFunction(Interpreter& interpreter, std::string importName, std::string functionName, std::vector<ska::NodeValue> parametersValues) {
			SLOG(LogLevel::Info) << "Looking for import \"" << importName << "\" in script \"" << m_script.name() << "\"";
			auto import = m_script.findInMemoryTree(importName);
			if (import.first == nullptr) {
				throw std::runtime_error("unable to find import \"" + importName + "\" queried in script \"" + m_script.name() + "\"");
			}
			const auto importedScriptId = import.first->template nodeval<ska::ScriptVariableRef>();
			auto importedScript = m_script.useImport(importedScriptId.script);
			auto functionToCallMemory = importedScript->downMemory()(functionName);
			auto& functionToExecute = functionToCallMemory.first->template nodeval<ska::ScriptVariableRef>();

			auto* stored = importedScript->getFunction(functionToExecute.variable);
			auto contextToExecute = ExecutionContext{ *importedScript, *stored->node, stored->memory };

			auto operateOnFunction = ska::Operation<ska::Operator::FUNCTION_DECLARATION>(contextToExecute);
			return ska::InterpreterOperationFunctionCallScriptWithParams(m_script, interpreter, functionToCallMemory.second, operateOnFunction, std::move(parametersValues)).asRvalue().object;
		}

		MemoryLValue accessMemory(std::string importName, std::string field) {
			SLOG(LogLevel::Info) << "Looking for import \"" << importName << "\" in script \"" << m_script.name() << "\"";
			auto found = m_script.findInMemoryTree(importName).first;
			if (found == nullptr) {
				throw std::runtime_error("unable to find import \"" + importName + "\" queried in script \"" + m_script.name() + "\"");
			}
			auto& emId = found->template nodeval<ska::ScriptVariableRef>();
			auto em = m_script.useImport(emId.script);
			if (em == nullptr) {
				throw std::runtime_error("unable to find import \"" + importName + "\" queried in script \"" + m_script.name() + "\"");
			}
			return em->downMemory()(field);
		}

	private:
		ScriptBridge<Interpreter>& m_binding;
		typename InterpreterTypes<Interpreter>::Script& m_script;
	};
}