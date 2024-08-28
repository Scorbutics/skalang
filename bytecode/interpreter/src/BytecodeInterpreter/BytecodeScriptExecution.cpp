#include "Config/LoggerConfigLang.h"
#include "BytecodeScriptExecution.h"
#include "BytecodeInterpreter/Value/BytecodeExecutor.h"

ska::NodeValue ska::bytecode::ScriptExecution::getCell(const Operand& v) const {
	if (v.empty()) {
		return {};
	}

	auto* memory = selectMemory(v);
	if (memory == nullptr) {
		auto output = NodeValue{};
		std::visit([&](const auto& content) {
			using TypeT = std::decay_t<decltype(content)>;
			if constexpr (std::is_same_v<ScriptVariableRef, TypeT>) {
				auto ss = std::stringstream{};
				ss << "invalid cell : memory type " << v.type();
				throw std::runtime_error(ss.str());
			} else {
				output = content;
			}
		}, v.content());
		return output;
	}
	assert(memory->size() > v.as<ScriptVariableRef>().variable);
	return (*memory)[v.as<ScriptVariableRef>().variable];
}

void ska::bytecode::ScriptExecution::jumpAbsolute(std::size_t value) {
	executionPointer = value;
}

void ska::bytecode::ScriptExecution::jumpRelative(long value) {
	executionPointer += value;
}

std::size_t ska::bytecode::ScriptExecution::id() const {
	return scriptIndex;
}

const std::string& ska::bytecode::ScriptExecution::name() const {
	return instructions.at(scriptIndex).name();
}

ska::bytecode::PlainMemoryTable* ska::bytecode::ScriptExecution::selectMemory(const Operand& dest) {
	return SelectMemoryHelper<decltype(*this), PlainMemoryTable*>(*this, dest);
}

const ska::bytecode::PlainMemoryTable* ska::bytecode::ScriptExecution::selectMemory(const Operand& dest) const {
	return SelectMemoryHelper<decltype(*this), const PlainMemoryTable*>(*this, dest);
}

void ska::bytecode::ScriptExecution::pushInEnv(const Operand& env, const Operand& variable) {
	const auto envIndex = env.as<ScriptVariableRef>().variable;
	if (envIndex >= captureEnvironment.size()) {
		captureEnvironment.resize(envIndex + 1);
	}
	captureEnvironment[envIndex].push_back(getCell(variable));
}

ska::NodeValue ska::bytecode::ScriptExecution::getInEnv(const Operand& env, std::size_t indexInEnv) const {
	const auto envIndex = env.as<ScriptVariableRef>().variable;
	assert(envIndex < captureEnvironment.size());
	assert(indexInEnv < captureEnvironment[envIndex].size());
	return captureEnvironment[envIndex][indexInEnv];
}
