#pragma once
#include <functional>
#include <cassert>
#include <unordered_map>

#include "NodeValue/ASTNodePtr.h"
#include "Interpreter/BridgeFunction.h"
#include "Utils/Observable.h"
#include "Utils/TupleUtils.h"
#include "Event/VarTokenEvent.h"
#include "Event/FunctionTokenEvent.h"
#include "Event/BlockTokenEvent.h"
#include "Event/ScriptLinkTokenEvent.h"
#include "Container/sorted_observable.h"

namespace ska {
	class SymbolTable;
	class TypeBuilder;
	class SymbolTableTypeUpdater;
	struct ReservedKeywordsPool;

	//http://coliru.stacked-crooked.com/a/8efdf80ac4082e22
	class BindingFactory :
		public observable_priority_queue<VarTokenEvent>,
		public observable_priority_queue<FunctionTokenEvent>,
		public observable_priority_queue<BlockTokenEvent>,
		public observable_priority_queue<ScriptLinkTokenEvent> {
	public:
		BindingFactory(TypeBuilder& typeBuilder, SymbolTableTypeUpdater& symbolTypeUpdater, const ReservedKeywordsPool& reserved);
		virtual ~BindingFactory();

		template <class ReturnType, class ... ParameterTypes>
		BridgeMemory bindFunction(Script& script, const std::string& functionName, std::function<ReturnType(ParameterTypes...)> f) {
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

	private:
		Token getTokenFromTypeName(const std::string& typeName);
		void unlisten(SymbolTable& symbolTable);
		void listen(SymbolTable& symbolTable);

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

		TypeBuilder& m_typeBuilder;
		SymbolTableTypeUpdater& m_symbolTypeUpdater;
        const ReservedKeywordsPool& m_reserved;
	};
}
