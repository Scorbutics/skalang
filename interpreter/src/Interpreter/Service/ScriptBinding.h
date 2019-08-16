#pragma once
#include <functional>
#include <cassert>
#include <vector>
#include <unordered_map>

#include "Container/sorted_observable.h"
#include "Interpreter/Value/BridgeFunction.h"
#include "Interpreter/Value/BridgeMemory.h"
#include "Service/BindingFactory.h"
#include "Event/VarTokenEvent.h"
#include "Interpreter/Value/Script.h"
#include "Interpreter/ScriptCache.h"

namespace ska {
	class SymbolTable;
	class StatementParser;
	class Interpreter;

	class ScriptBridge :
        protected observable_priority_queue<VarTokenEvent> {
	public:
		ScriptBridge(
			ScriptCache& cache,
			std::string scriptName,
			TypeBuilder& typeBuilder,
			SymbolTableUpdater& symbolTypeUpdater,
			const ReservedKeywordsPool& reserved);

		virtual ~ScriptBridge();

		void buildFunctions();

		template <class ReturnType, class ... ParameterTypes>
		void bindFunction(const std::string& functionName, std::function<ReturnType(ParameterTypes...)> f) {
			assert(m_scriptMemoryNode == nullptr);
			m_bindings.push_back(bindFunction_<ReturnType, ParameterTypes...>(m_script.astScript(), functionName, std::move(f)));
		}

		void bindGenericFunction(const std::string& functionName, std::vector<std::string> typeNames, decltype(BridgeFunction::function) f) {
			assert(m_scriptMemoryNode == nullptr);
			m_bindings.push_back(bindGenericFunction_(m_script.astScript(), functionName, std::move(typeNames), std::move(f)));
		}

		void import(StatementParser& parser, Interpreter& interpreter, std::vector<std::pair<std::string, std::string>> imports);

		MemoryTablePtr createMemory() { return m_script.createMemory(); }

		NodeValue callFunction(Interpreter& interpreter, std::string importName, std::string functionName, std::vector<ska::NodeValue> parametersValues);
		MemoryLValue accessMemory(std::string importName, std::string field);

	private:
		template <class ReturnType, class ... ParameterTypes>
		BridgeMemory bindFunction_(ScriptAST& script, const std::string& functionName, std::function<ReturnType(ParameterTypes...)> f) {
			auto result = makeScriptSideBridge(std::move(f));
			auto typeNames = m_functionBinder.template buildTypes<ParameterTypes..., ReturnType>();
			result->node = m_functionBinder.bindSymbol(script, functionName, std::move(typeNames));
			return result;
		}

		BridgeMemory bindGenericFunction_(ScriptAST& script, const std::string& functionName, std::vector<std::string> typeNames, decltype(BridgeFunction::function) f) {
			auto result = std::make_unique<BridgeFunction>(std::move(f));
			result->node = m_functionBinder.bindSymbol(script, functionName, std::move(typeNames));
			return result;
		}

		template <class ReturnType, class ... ParameterTypes, std::size_t... Idx>
		auto callNativeFromScript(std::function<ReturnType(ParameterTypes...)> f, const std::vector<NodeValue>& v, std::index_sequence<Idx...>) {
			return f(convertTypeFromScript<ParameterTypes, Idx>(v)...);
		}

		template <class ReturnType, class ... ParameterTypes>
		BridgeMemory makeScriptSideBridge(std::function<ReturnType(ParameterTypes...)> f) {
			auto lambdaWrapped = [f, this](std::vector<NodeValue> v) {
				if constexpr(std::is_same_v<ReturnType, void>) {
					callNativeFromScript(std::move(f), v, std::make_index_sequence<sizeof ...(ParameterTypes)>());
					return NodeValue{};
				} else {
					return NodeValue(callNativeFromScript(std::move(f), v, std::make_index_sequence<sizeof ...(ParameterTypes)>()));
				}
			};

			return std::make_unique<BridgeFunction>(static_cast<decltype(BridgeFunction::function)> (std::move(lambdaWrapped)));
		}

		template <class T, std::size_t Id>
		T convertTypeFromScript(const std::vector<NodeValue>& vect) {
			assert(Id < vect.size());
			const auto& v = vect[Id];
			if constexpr (std::is_same<T, StringShared>()) {
				return std::make_shared<std::string>(v.convertString());
			} else if constexpr (std::is_same<T, int>()) {
				return static_cast<T>(v.convertNumeric());
			} else if constexpr (std::is_same<T, std::size_t>()) {
				return static_cast<T>(v.convertNumeric());
			} else if constexpr (std::is_same<T, float>()) {
				return static_cast<T>(v.convertNumeric());
			} else if constexpr (std::is_same<T, bool>()) {
				return static_cast<int>(v.convertNumeric()) != 0;
			} else if constexpr (std::is_same<T, double>()) {
				return static_cast<T>(v.convertNumeric());
			} else {
				throw std::runtime_error("Invalid type for bridge function");
			}
		}

		TypeBuilder& m_typeBuilder;
		SymbolTableUpdater& m_symbolTypeUpdater;
		BindingFactory m_functionBinder;
		std::string m_name;
		Script m_script;
		ASTNodePtr m_scriptMemoryNode;
		ScriptCache& m_cache;

		std::vector<ASTNodePtr> m_imports;
		std::vector<BridgeMemory> m_bindings;
	};
}
