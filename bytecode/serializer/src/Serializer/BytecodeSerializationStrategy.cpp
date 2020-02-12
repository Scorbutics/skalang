#include <string>
#include "BytecodeSerializationStrategy.h"

template <class T>
T& build(const std::string& scriptName) {
	auto bytecodeScriptName = scriptName + "b";
	static auto tabs = std::unordered_map<std::string, T> {};
	auto tabFound = tabs.find(bytecodeScriptName);
	if (tabFound == tabs.end()) {
		tabFound = tabs.emplace(bytecodeScriptName, T{}).first;
		tabFound->second.open(bytecodeScriptName, std::iostream::binary);
		if (tabFound->second.fail()) {
			throw std::runtime_error("opening file " + bytecodeScriptName + " failed");
		}
	}
	return tabFound->second;
}

ska::bytecode::SerializationStrategy ska::bytecode::SerializationStrategyType::PerScript() {
	return [](const std::string& scriptName) -> std::ofstream& {
		return build<std::ofstream>(scriptName);
	};
}

ska::bytecode::DeserializationStrategy ska::bytecode::DeserializationStrategyType::PerScript() {
	return [](const std::string& scriptName) -> std::ifstream& {
		return build<std::ifstream>(scriptName);
	};
}

