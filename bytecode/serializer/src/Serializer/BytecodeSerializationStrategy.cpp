#include <string>
#include "BytecodeSerializationStrategy.h"

template <class T>
T& build(std::size_t scriptId) {
	static std::vector<T> tabs;
	if (scriptId >= tabs.size()) {
		tabs.resize(tabs.size() + 1);
		auto scriptName = "bytecode_" + std::to_string(scriptId);
		tabs[scriptId].open(scriptName, std::iostream::binary);
		if (tabs[scriptId].fail()) {
			throw std::runtime_error("opening file " + scriptName + " failed");
		}
	}
	return tabs[scriptId];
}

ska::bytecode::SerializationStrategy ska::bytecode::SerializationStrategyType::PerScript() {
	static auto result = [](std::size_t scriptId) -> std::ofstream& {
		return build<std::ofstream>(scriptId);
	};
	return result;
}

ska::bytecode::DeserializationStrategy ska::bytecode::DeserializationStrategyType::PerScript() {
	static auto result = [](std::size_t scriptId) -> std::ifstream& {
		return build<std::ifstream>(scriptId);
	};
	return result;
}

