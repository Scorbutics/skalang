#include "BytecodeScriptExecution.h"
#include "BytecodeInterpreter/Value/BytecodeExecutionOutput.h"

ska::bytecode::NodeValue ska::bytecode::ScriptExecution::getCell(const Value& v) const {
	if (v.empty()) {
		return {};
	}

	auto* memory = selectMemory(v);
	if (memory == nullptr) {
		auto output = NodeValue{};
		std::visit([&](const auto& content) {
			using TypeT = std::decay_t<decltype(content)>;
			if constexpr (std::is_same_v<VariableRef, TypeT>) {
				throw std::runtime_error("invalid cell");
			}
			else {
				output = content;
			}
		}, v.content());
		return output;
	}
	return (*memory)[std::get<std::size_t>(v.as<VariableRef>())];
}

void ska::bytecode::ScriptExecution::jumpReturn() {
	assert(!execution.callstack.empty());
	executionPointer = execution.callstack.back().nodeval<std::size_t>();
	execution.callstack.pop_back();
}

void ska::bytecode::ScriptExecution::jumpAbsolute(std::size_t value) {
	execution.callstack.push_back(TokenVariant{ executionPointer });
	executionPointer = value - 1;
}

void ska::bytecode::ScriptExecution::jumpRelative(long value) {
	executionPointer += value;
}

ska::bytecode::PlainMemoryTable* ska::bytecode::ScriptExecution::selectMemory(const Value& dest) {
	return SelectMemoryHelper<decltype(*this), PlainMemoryTable*>(*this, dest);
}

const ska::bytecode::PlainMemoryTable* ska::bytecode::ScriptExecution::selectMemory(const Value& dest) const {
	return SelectMemoryHelper<decltype(*this), const PlainMemoryTable*>(*this, dest);
}
