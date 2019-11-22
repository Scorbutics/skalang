#include "Config/LoggerConfigLang.h"
#include "BytecodeInstructionOutput.h"
#include "BytecodeScriptGenerationOutput.h"

std::ostream& ska::bytecode::operator<<(std::ostream& stream, const InstructionPack& group) {
	for(const auto& c : group) {
		stream << c << "\n";
	}
	return stream;
}

ska::bytecode::InstructionOutput::InstructionOutput(ScriptGenerationOutput script) :
	InstructionOutput(std::move(script.m_generated)) {
}

void ska::bytecode::InstructionOutput::push(InstructionOutput value) {
	const auto wasEmpty = value.m_pack.empty();
	if(!wasEmpty) {
		m_pack.insert(m_pack.end(), std::make_move_iterator(value.m_pack.begin()), std::make_move_iterator(value.m_pack.end()));
		m_value = {};
	} else {
		m_value = value.m_value;
	}
}

void ska::bytecode::InstructionOutput::push(Value value) {
	push( {std::move(value) });
}

void ska::bytecode::InstructionOutput::push(Instruction value) {
	push( {std::move(value) });
}

std::ostream& ska::bytecode::operator<<(std::ostream& stream, const InstructionOutput& output) {
	if(output.m_pack.empty()) {
		if(!output.m_value.empty()) {
			stream << output.m_value.toString();
		}
	} else {
		stream << output.m_pack << " ";
	}
	return stream;
}

ska::bytecode::Value ska::bytecode::InstructionOutput::packAsValue() const {
	if(m_pack.empty()) {
		return {};
	}

	return m_pack.back().dest();
}
