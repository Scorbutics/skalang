#pragma once
#include <memory>
#include "NodeValue/Type.h"
#include "NodeValue.h"
#include "Service/SymbolTable.h"
#include "Service/IsSmartPtr.h"
#include "InterpreterTypes.h"

namespace ska {

	template <class Interpreter>
	struct BridgeMemory {
		using Object = typename InterpreterTypes<Interpreter>::Memory;

		BridgeMemory(Object&& memory, const Symbol& symbol) :
			memory(std::forward<Object>(memory)),
			symbol(&symbol) {
			setupReferenceMemory();
		}

		BridgeMemory(Object&& memory, const SymbolTable& symbols) :
			memory(std::forward<Object>(memory)),
			symbols(&symbols) {
			setupReferenceMemory();
		}

		void setupReferenceMemory() {
			if constexpr (is_smart_ptr<Object>::value || std::is_pointer_v<Object>) {
				refMemory = &(*memory);
			} else {
				refMemory = &memory;
			}
		}

		template <class Value>
		void push(const std::string& field, Value&& obj) {
			/*if(symbol != nullptr && (*symbol)[field] == nullptr || symbol == nullptr && (*symbols)[field] == nullptr) {
				throw std::runtime_error("unable to replace the field \"" + field + "\" while binding this script because it is not found as declared in any external script");
			}*/
			refMemory->emplace(field, std::forward<Value>(obj));
		}

		NodeValue value() const {
			return refMemory->value();
		}

	private:
		Object memory;
		Object* refMemory = nullptr;

		const Symbol* symbol = nullptr;
		const SymbolTable* symbols = nullptr;
	};

}