#pragma once

#include <functional>
#include <cassert>
#include <vector>
#include <unordered_map>

#include "Container/sorted_observable.h"
#include "Runtime/Value/BridgeFunction.h"
#include "BindingFactory.h"
#include "Event/VarTokenEvent.h"
#include "NodeValue/ScriptCacheAST.h"
#include "NodeValue/ScriptASTPtr.h"
#include "NodeValue/ScriptAST.h"
#include "Runtime/Value/NodeValue.h"
#include "Runtime/Value/InterpreterTypes.h"
#include "Runtime/Value/ModuleConfiguration.h"

namespace ska {
	class ScriptBindingBase;
}

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::ScriptBindingBase);

namespace ska {
	class SymbolTable;
	class StatementParser;

	class ScriptBindingBase :
        protected observable_priority_queue<VarTokenEvent> {
	public:
		ScriptBindingBase (
			StatementParser& parser,
      ScriptCacheAST& cache,
			std::string scriptName,
			std::string templateScriptName,
			TypeBuilder& typeBuilder,
			SymbolTableUpdater& symbolTypeUpdater,
			const ReservedKeywordsPool& reserved);

		virtual ~ScriptBindingBase();

		template <class ReturnType, class ... ParameterTypes>
		void bindFunction(const std::string& functionName, std::function<ReturnType(ParameterTypes...)> f) {
			m_bindings.push_back(bindFunction_<ReturnType, ParameterTypes...>(m_script, functionName, std::move(f)));
		}

		void bindGenericFunction(const std::string& functionName, std::vector<std::string> typeNames, decltype(BridgeFunction::function) f) {
			m_bindings.push_back(bindGenericFunction_(m_script, functionName, std::move(typeNames), std::move(f)));
		}

		void bindGenericFunction(const Type& fullFunctionType, decltype(BridgeFunction::function) f) {
			m_bindings.push_back(bindGenericFunction_(m_script, fullFunctionType, std::move(f)));
		}

		const auto& name() const { return m_script.name(); }
		const std::string& templateName() const;

	protected:
		BridgeImport import(std::string constructorMethod, StatementParser& parser, std::pair<std::string, std::string> import);

		template <class Interpreter>
		void buildFunctions(Interpreter& interpreter, typename InterpreterTypes<Interpreter>::Script& script) {
      SLOG(LogLevel::Info) << "Building script " << script.astScript().name() << " ( " << m_script.name() << ") from bridge";

			assert(!m_bindings.empty() && "Bridge is empty");
      assert(script.astScript().id() == m_script.id());

			auto& scriptAstNode = m_script.fromBridge(m_bindings);
      registerAST(scriptAstNode);

			assert(m_templateScript != nullptr && "Template script was not AST-generated");
			//script.memoryFromBridge(*m_templateScript, interpreter, std::move(m_bindings));

      m_bindings = { };
    }

		void generateAst();

	private:
		template <class ReturnType, class ... ParameterTypes>
		BridgeFunctionPtr bindFunction_(ScriptAST& script, const std::string& functionName, std::function<ReturnType(ParameterTypes...)> f) {
			auto result = makeScriptSideBridge(std::move(f));
			auto typeNames = m_functionBinder.template buildTypes<ParameterTypes..., ReturnType>();
			result->node = m_functionBinder.bindFunction(script, functionName, std::move(typeNames));
			return result;
		}

		BridgeFunctionPtr bindGenericFunction_(ScriptAST& script, const std::string& functionName, std::vector<std::string> typeNames, decltype(BridgeFunction::function) f) {
			auto result = std::make_shared<BridgeFunction>(std::move(f));
			result->node = m_functionBinder.bindFunction(script, functionName, std::move(typeNames));
			return result;
		}

		BridgeFunctionPtr bindGenericFunction_(ScriptAST& script, const Type& fullFunctionType, decltype(BridgeFunction::function) f) {
			auto result = std::make_shared<BridgeFunction>(std::move(f));
			result->node = m_functionBinder.bindFunction(script, fullFunctionType);
			return result;
		}

		template <class ReturnType, class ... ParameterTypes, std::size_t... Idx>
		auto callNativeFromScript(std::function<ReturnType(ParameterTypes...)> f, const std::vector<NodeValue>& v, std::index_sequence<Idx...>) {
			return f(convertTypeFromScript<ParameterTypes, Idx>(v)...);
		}

		template <class ReturnType, class ... ParameterTypes>
		BridgeFunctionPtr makeScriptSideBridge(std::function<ReturnType(ParameterTypes...)> f) {
			auto lambdaWrapped = [f, this](std::vector<NodeValue> v) {
				if constexpr(std::is_same_v<ReturnType, void>) {
					callNativeFromScript(std::move(f), v, std::make_index_sequence<sizeof ...(ParameterTypes)>());
					return NodeValue{};
				} else {
					return NodeValue(callNativeFromScript(std::move(f), v, std::make_index_sequence<sizeof ...(ParameterTypes)>()));
				}
			};

			return std::make_shared<BridgeFunction>(static_cast<decltype(BridgeFunction::function)> (std::move(lambdaWrapped)));
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

    void registerAST(ASTNode& scriptAst);

	protected:
		StatementParser& m_parser;
		ScriptASTPtr m_templateScript;

	private:
		TypeBuilder& m_typeBuilder;
		SymbolTableUpdater& m_symbolTypeUpdater;
		const ReservedKeywordsPool& m_reservedKeywordsPool;
		BindingFactory m_functionBinder;
		std::string m_name;
		std::string m_templateName;
		ScriptAST m_script;
		ScriptCacheAST& m_cache;

		std::unordered_map<std::string, ASTNodePtr> m_imports;
		std::vector<BridgeFunctionPtr> m_bindings;
	};

	template <class Interpreter>
	class ScriptBinding : public ScriptBindingBase {
	using ScriptCache = typename InterpreterTypes<Interpreter>::ScriptCache;
	using Script = typename InterpreterTypes<Interpreter>::Script;
	using ModuleConfiguration = lang::ModuleConfiguration<Interpreter>;
	public:
		ScriptBinding(
			ModuleConfiguration& moduleConf,
			std::string scriptName,
			std::string templateScriptName) :
			ScriptBindingBase(moduleConf.parser, moduleConf.scriptAstCache, scriptName, std::move(templateScriptName), moduleConf.typeBuilder, moduleConf.symbolTableUpdater, moduleConf.reservedKeywords),
			m_interpreter(moduleConf.interpreter),
			m_script(moduleConf.scriptCache, ScriptBindingBase::name(), std::vector<Token>{}) {
		}

		void buildFunctions() {
			ScriptBindingBase::buildFunctions(m_interpreter, m_script);
		}

		ska::BridgeImport generateTemplate(std::string constructorMethod) {
			auto result = import(std::move(constructorMethod), { "BASE", ScriptBindingBase::templateName() });
			ScriptBindingBase::generateAst();
			/*
			bindGenericFunction("Fcty", { "void", m_templateScript->symbols()[""] }, std::function<ska::NodeValue(std::vector<ska::NodeValue>)>([&](std::vector<ska::NodeValue> buildParams) -> ska::NodeValue {
					auto memory = createMemory();
				return ska::NodeValue{ std::move(memory) };
			} );
			ScriptBindingBase::buildFunctions(m_interpreter, m_script);
			*/
			return result;
		}

		auto& script() { return m_script; }

		BridgeImport import(std::string constructorMethod, std::pair<std::string, std::string> import) {
			auto importRef = ScriptBindingBase::import(std::move(constructorMethod), m_parser, std::move(import));
			m_interpreter.interpret({ m_script, *importRef.node });
			return importRef;
		}

	private:
		Script m_script;
		Interpreter& m_interpreter;
	};
}
