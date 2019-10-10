#pragma once
#include <memory>
#include "NodeValue/Type.h"
#include "Service/SymbolTable.h"

namespace ska {
	struct BridgeFunction;
	using BridgeFunctionPtr = std::shared_ptr<BridgeFunction>;

	template <class Object>
	struct BridgeMemory {
		BridgeMemory(Object&& memory, const Symbol& symbol) :
			memory(std::forward<Object>(memory)),
			symbol(&symbol) {}

		BridgeMemory(Object&& memory, const SymbolTable& symbols) :
			memory(std::forward<Object>(memory)),
			symbols(&symbols) {}

		template <class Value>
		void replace(const std::string& field, Value&& obj) {
			/*if(symbol != nullptr && (*symbol)[field] == nullptr || symbol == nullptr && (*symbols)[field] == nullptr) {
				throw std::runtime_error("unable to replace the field \"" + field + "\" while binding this script because it is not found as declared in any external script");
			}*/
			memory->emplace(field, std::forward<Value>(obj));
		}

		Object memory;
	private:
		const Symbol* symbol = nullptr;
		const SymbolTable* symbols = nullptr;
	};
}