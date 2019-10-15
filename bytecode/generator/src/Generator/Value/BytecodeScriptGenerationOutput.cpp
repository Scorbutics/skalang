#include "Config/LoggerConfigLang.h"
#include "BytecodeScriptGenerationOutput.h"

std::ostream& ska::bytecode::operator<<(std::ostream& stream, const InstructionPack& group) {
	for(const auto& c : group) {
		stream << c << "\n";
	}
	return stream;
}

void ska::bytecode::ScriptGenerationOutput::push(ScriptGenerationOutput value) {
	const auto wasEmpty = value.m_pack.empty();
	if(!wasEmpty) {
		m_pack.insert(m_pack.end(), std::make_move_iterator(value.m_pack.begin()), std::make_move_iterator(value.m_pack.end()));
		m_value = {};
	} else {
		m_value = value.m_value;
	}
}

std::ostream& ska::bytecode::operator<<(std::ostream& stream, const ScriptGenerationOutput& output) {
	if(output.m_pack.empty()) {
		if(!output.m_value.empty()) {
			stream << output.m_value.toString();
		}
	} else {
		stream << output.m_pack << " ";
	}
	//stream << output.m_symbolsPack;
	return stream;
}

ska::bytecode::Value ska::bytecode::ScriptGenerationOutput::packAsValue() const {
	if(m_pack.empty()) {
		return {};
	}

	return m_pack.back().dest();
}