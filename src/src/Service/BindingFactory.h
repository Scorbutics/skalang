#pragma once
#include <functional>
#include <cassert>
#include <unordered_map>

#include "NodeValue/ASTNodePtr.h"
#include "Interpreter/Value/BridgeFunction.h"
#include "Base/Patterns/Observable.h"
#include "Base/Meta/TupleUtils.h"
#include "Event/VarTokenEvent.h"
#include "Event/FunctionTokenEvent.h"
#include "Event/BlockTokenEvent.h"
#include "Event/ImportTokenEvent.h"
#include "Event/ScriptLinkTokenEvent.h"
#include "Container/sorted_observable.h"
#include "Interpreter/ScriptCache.h"

#include "Service/Matcher/MatcherType.h"

namespace ska {
	class SymbolTable;
	class TypeBuilder;
	class SymbolTableTypeUpdater;
	struct ReservedKeywordsPool;
	class StatementParser;
	class Interpreter;

	class BindingFactory;
	class BindingFactorySymbolTableLock {	
	public:
		BindingFactorySymbolTableLock(BindingFactory& factory, SymbolTable& table);
		BindingFactorySymbolTableLock(BindingFactorySymbolTableLock&&) noexcept;
		BindingFactorySymbolTableLock(const BindingFactorySymbolTableLock&) = delete;
		BindingFactorySymbolTableLock& operator=(BindingFactorySymbolTableLock&&) = delete;
		BindingFactorySymbolTableLock& operator=(const BindingFactorySymbolTableLock&) = delete;

		~BindingFactorySymbolTableLock();
		void release();
	private:
		BindingFactory& m_factory;
		SymbolTable& m_symbolTable;
		bool m_freed = false;
	};

	//http://coliru.stacked-crooked.com/a/8efdf80ac4082e22
	class BindingFactory :
		public observable_priority_queue<VarTokenEvent>,
		public observable_priority_queue<FunctionTokenEvent>,
		public observable_priority_queue<BlockTokenEvent>,
		public observable_priority_queue<ScriptLinkTokenEvent>,
		public observable_priority_queue<ImportTokenEvent> {
		friend class BindingFactorySymbolTableLock;
	public:
		BindingFactory(TypeBuilder& typeBuilder, SymbolTableTypeUpdater& symbolTypeUpdater, const ReservedKeywordsPool& reserved);
		virtual ~BindingFactory();

		template <class ReturnType, class ... ParameterTypes>
		BridgeMemory bindFunction(Script& script, const std::string& functionName, std::function<ReturnType(Script&, ParameterTypes...)> f) {
			auto typeNames = buildTypes<ParameterTypes..., ReturnType>();
			auto result = makeScriptSideBridge(std::move(f));
			result->node = bindSymbol(script, functionName, std::move(typeNames));
			return result;
		}

		BridgeMemory bindGenericFunction(Script& script, const std::string& functionName, std::vector<std::string> typeNames, decltype(BridgeFunction::function) f) {
			auto result = std::make_unique<BridgeFunction>(std::move(f));
			result->node = bindSymbol(script, functionName, std::move(typeNames));
			return result;
		}

		ASTNodePtr import(StatementParser& parser, Script& script, Interpreter& interpreter, std::vector<std::pair<std::string, std::string>> imports);

	private:
		void internalUnlisten(SymbolTable& symbolTable);
		void internalListen(SymbolTable& symbolTable);

		template <class ReturnType, class ... ParameterTypes, std::size_t... Idx>
		auto callNativeFromScript(Script& caller, std::function<ReturnType(Script&, ParameterTypes...)> f, const std::vector<NodeValue>& v, std::index_sequence<Idx...>) {
			return f(caller, convertTypeFromScript<ParameterTypes, Idx>(v)...);
		}

		template <class ReturnType, class ... ParameterTypes>
		BridgeMemory makeScriptSideBridge(std::function<ReturnType(Script&, ParameterTypes...)> f) {
			auto lambdaWrapped = [f, this](Script& caller, std::vector<NodeValue> v) {
				if constexpr(std::is_same_v<ReturnType, void>) {
					callNativeFromScript(caller, std::move(f), v, std::make_index_sequence<sizeof ...(ParameterTypes)>());
					return NodeValue{};
				} else {
					return NodeValue(callNativeFromScript(caller, std::move(f), v, std::make_index_sequence<sizeof ...(ParameterTypes)>()));
				}
			};

			return std::make_unique<BridgeFunction>(static_cast<decltype(BridgeFunction::function)> (std::move(lambdaWrapped)));
		}

		template <class T, std::size_t Id>
		T convertTypeFromScript(const std::vector<NodeValue>& vect) {
			assert(Id < vect.size());
			const auto& v = vect[Id];
			if constexpr (std::is_same<T, std::string>()) {
				return v.convertString();
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
				assert(!"Invalid type for bridge function");
				return T{};
			}
		}

		template <class T>
		void buildType(std::vector<std::string>& ss) {
			if constexpr (std::is_same<T, std::string>()) {
				ss.push_back("string");
			} else if constexpr (std::is_same<T, int>()) {
				ss.push_back("int");
			} else if constexpr (std::is_same<T, std::size_t>()) {
				ss.push_back("int");
			} else if constexpr (std::is_same<T, float>()) {
				ss.push_back("float");
			} else if constexpr (std::is_same<T, bool>()) {
				ss.push_back("bool");
			} else if constexpr (std::is_same<T, double>()) {
				ss.push_back("float");
			} else if constexpr (std::is_same<T, void>()) {
				ss.push_back("void");
			} else {
				assert(!"Invalid type for bridge function");
			}
		}

		template<class ... Types>
		std::vector<std::string> buildTypes() {
			auto ss = std::vector<std::string>{};
			int _[] = {0, (buildType<Types>(ss), 0)...};
			(void)_;
			return ss;
		}

		ASTNodePtr bindSymbol(Script& script, const std::string& functionName, std::vector<std::string> typeNames);
		ASTNodePtr createImport(StatementParser& parser, Script& script, Token scriptPathToken);

		TypeBuilder& m_typeBuilder;
		SymbolTableTypeUpdater& m_symbolTypeUpdater;
        const ReservedKeywordsPool& m_reserved;
		MatcherType m_matcherType;
	};
}
