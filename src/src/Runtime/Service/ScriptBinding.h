#pragma once

#include <functional>
#include <cassert>
#include <vector>
#include <unordered_map>

#include "Runtime/Value/NativeFunction.h"
#include "BridgeASTBuilder.h"
#include "NodeValue/ScriptCacheAST.h"
#include "NodeValue/ScriptASTPtr.h"
#include "NodeValue/ScriptAST.h"
#include "Runtime/Value/NodeValue.h"
#include "Runtime/Value/InterpreterTypes.h"
#include "Runtime/Value/ModuleConfiguration.h"
#include "Runtime/Service/BridgeFunction.h"

namespace ska {
	class ScriptBindingAST;
}

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::ScriptBindingAST);

namespace ska {
	class SymbolTable;
	class StatementParser;

	class ScriptBindingAST {
	public:
		ScriptBindingAST(
			StatementParser& parser,
      ScriptCacheAST& cache,
			std::string scriptName,
			std::string templateScriptName,
			TypeBuilder& typeBuilder,
			SymbolTableUpdater& symbolTypeUpdater,
			const ReservedKeywordsPool& reserved);

		virtual ~ScriptBindingAST() = default;

		void bindFunction(Type functionType, decltype(NativeFunction::function) f);

		std::size_t id() const { return m_scriptAst.id(); }
		const auto& name() const { return m_scriptAst.name(); }
		const std::string& templateName() const;
		auto& templateScript() { return *m_templateScript; }

	private:
		void queryAST();

		template <class ReturnType, class ... ParameterTypes, std::size_t... Idx>
		auto callNativeFromScript(std::function<ReturnType(ParameterTypes...)> f, const std::vector<NodeValue>& v, std::index_sequence<Idx...>) {
			return f(convertTypeFromScript<ParameterTypes, Idx>(v)...);
		}

		template <class ReturnType, class ... ParameterTypes>
		NativeFunctionPtr makeScriptSideBridge(std::function<ReturnType(ParameterTypes...)> f) {
			auto lambdaWrapped = [f, this](std::vector<NodeValue> v) {
				if constexpr(std::is_same_v<ReturnType, void>) {
					callNativeFromScript(std::move(f), v, std::make_index_sequence<sizeof ...(ParameterTypes)>());
					return NodeValue{};
				} else {
					return NodeValue(callNativeFromScript(std::move(f), v, std::make_index_sequence<sizeof ...(ParameterTypes)>()));
				}
			};

			return std::make_shared<NativeFunction>(static_cast<decltype(NativeFunction::function)> (std::move(lambdaWrapped)));
		}

		template <class T, std::size_t Id>
		T convertTypeFromScript(const std::vector<NodeValue>& vect) {
			assert(Id < vect.size());
			const auto& v = vect[Id];
			if constexpr (std::is_same<T, StringShared>()) {
				return std::make_shared<std::string>(v.convertString());
			} else if constexpr (std::is_same<T, int>()) {
				return static_cast<T>(v.convertNumeric());
			} else if constexpr (std::is_same<T, long>()) {
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

	protected:
		ASTNodePtr buildFunctionsAST(const BridgeFunction& constructor);
		BridgeFunction buildConstructorFromBindings(BridgeField constructorField);
		StatementParser& m_parser;
		ScriptASTPtr m_templateScript;

	private:
		TypeBuilder& m_typeBuilder;
		SymbolTableUpdater& m_symbolTypeUpdater;
		const ReservedKeywordsPool& m_reservedKeywordsPool;
		BridgeASTBuilder m_functionBuilder;
		std::string m_name;
		std::string m_templateName;
		ScriptAST m_scriptAst;
		ScriptCacheAST& m_cacheAst;

		std::unordered_map<std::string, ASTNodePtr> m_imports;
		std::vector<BridgeField> m_bindings;
	};

	template <class Interpreter>
	class ScriptBinding : public ScriptBindingAST {
	using ScriptCache = typename InterpreterTypes<Interpreter>::ScriptCache;
	using Script = typename InterpreterTypes<Interpreter>::Script;
	using ModuleConfiguration = lang::ModuleConfiguration<Interpreter>;
	public:
		ScriptBinding(
			ModuleConfiguration& moduleConf,
			std::string scriptName,
			std::string templateScriptName) :
			ScriptBindingAST(moduleConf.parser, moduleConf.scriptAstCache, scriptName, std::move(templateScriptName), moduleConf.typeBuilder, moduleConf.symbolTableUpdater, moduleConf.reservedKeywords),
			m_interpreter(moduleConf.interpreter),
			m_script(moduleConf.scriptCache, ScriptBindingAST::name(), std::vector<Token>{}) {
			if (ScriptBindingAST::id() != m_script.astScript().id()) {
				throw std::runtime_error("script index coherence error while binding a script");
			}
		}

		auto& script() { return m_script; }

		void buildFunctions(BridgeField constructorField) {
			auto constructor = ScriptBindingAST::buildConstructorFromBindings(std::move(constructorField));
			auto astRoot = ScriptBindingAST::buildFunctionsAST(constructor);
			m_script.fromBridge(constructor, std::move(astRoot), m_interpreter);
		}

	private:
		Script m_script;
		Interpreter& m_interpreter;
	};
}
