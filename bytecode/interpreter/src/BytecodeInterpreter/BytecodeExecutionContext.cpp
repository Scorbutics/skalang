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
		}, v.content());
		return output;
	}
	return (*memory)[std::get<std::size_t>(v.as<VariableRef>())];
}

void ska::bytecode::ExecutionContext::jumpReturn() {
	assert(!callstack.empty());
	executionPointer = callstack.back().nodeval<std::size_t>();
	callstack.pop_back();
}

void ska::bytecode::ExecutionContext::jumpAbsolute(std::size_t value) {
	callstack.push_back(TokenVariant{ executionPointer });
	executionPointer = value - 1;
}

void ska::bytecode::ExecutionContext::jumpRelative(long value) {
	executionPointer += value;
}
