#include "BytecodeExecutionContext.h"

ska::bytecode::NodeValue ska::bytecode::ExecutionContext::getCell(const Value& v) {
	if (v.empty()) {
		return {};
	}

	auto* memory = selectMemory(v);
	if(memory == nullptr) {
		auto output = NodeValue{};
		std::visit([&](const auto& content) {
			using TypeT = std::decay_t<decltype(content)>;
			if constexpr (std::is_same_v<VariableRef, TypeT>) {
				throw std::runtime_error("invalid cell");
			} else {
				output = content;
			}
		}, v.content);
		return output;
	}
	return (*memory)[std::get<std::size_t>(v.as<VariableRef>())];
}
