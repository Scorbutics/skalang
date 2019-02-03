#pragma once
#include <functional>
#include "Interpreter/BridgeFunction.h"
#include "Utils/Observable.h"
#include "Utils/TupleUtils.h"
#include "Event/VarTokenEvent.h"
#include "Event/FunctionTokenEvent.h"

namespace ska {
	class SymbolTable;
	class Interpreter;
	
	//http://coliru.stacked-crooked.com/a/8efdf80ac4082e22
	class Binding :
		public Observable<VarTokenEvent>,
		public Observable<FunctionTokenEvent> {
	public:
		Binding(Interpreter& interpreter, SymbolTable& symbolTable);
		~Binding() = default;

		template <class ReturnType, class ... ParameterTypes>
		ska::BridgeFunctionPtr bindFunction(const std::string& functionName, std::function<ReturnType(ParameterTypes...)> f) {
			auto typeNames = buildTypes<ReturnType, ParameterTypes...>();
			auto result = makeScriptSideBridge<ReturnType, ParameterTypes...>(std::move(f));
			bindSymbol(functionName, std::move(typeNames));
			return result;
		}

	private:
		template <class ReturnType, class ... ParameterTypes, std::size_t... Idx>
		auto callNativeFromScript(std::function<ReturnType(ParameterTypes...)> f, std::vector<NodeValue>& v, std::index_sequence<Idx...>) {
			return f(convertTypeFromScript<ParameterTypes, Idx>(v)...);
		}

		template <class ReturnType, class ... ParameterTypes>
		BridgeFunctionPtr makeScriptSideBridge(std::function<ReturnType(ParameterTypes...)> f) {
			return std::make_unique<BridgeFunction>(static_cast<decltype(BridgeFunction::function)> ([&f, this](std::vector<NodeValue> v) {
				return NodeValue{ callNativeFromScript<ReturnType, ParameterTypes...>(std::move(f), v, std::make_index_sequence<sizeof ...(ParameterTypes)>()) };
			}) );
		}

		template <class T, std::size_t Id>
		T convertTypeFromScript(const std::vector<NodeValue>& vect) {
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
				return T{};
                //static_assert(false, "Invalid type for bridge function");
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
			} else {
                //static_assert(false, "Invalid type for bridge function");
			}
		}

		template<class ... Types>
		std::vector<std::string> buildTypes() {
			auto types = std::tuple<Types...>{};
			auto ss = std::vector<std::string>{};
			int _[] = {0, (buildType<Types>(ss), 0)...};
			(void)_;
			return ss;
		}

		void bindSymbol(const std::string& functionName, std::vector<std::string> typeNames);

		Interpreter& m_interpreter;
		Observer<VarTokenEvent>& m_observer;
		std::vector<ASTNodePtr> m_bridges;
	};
}
