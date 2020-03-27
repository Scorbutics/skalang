#include "Config/LoggerConfigLang.h"
#include "BytecodeSerializationContext.h"
#include "BytecodeCommonSerializer.h"
#include "BytecodeOperandSerializer.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::SerializationContext);

#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::SerializationContext)
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::bytecode::SerializationContext)

ska::bytecode::SerializationContext::SerializationContext(const ScriptCache& cache, SerializationStrategy strategy) :
	m_strategy(std::move(strategy)),
	m_cache(cache),
	m_output(&m_strategy(cache.at(m_id).name())),
	m_symbolsSerializer(cache) {
}

bool ska::bytecode::SerializationContext::next(std::deque<std::size_t> partIndexes) {
	partIndexes.push_front(pushNatives());
	commit(std::move(partIndexes));
	m_id++;
	if (m_cache.exist(m_id)) {
		m_output = &m_strategy(m_cache[m_id].name());
		return true;
	}
	return false;
}

std::size_t ska::bytecode::SerializationContext::writeHeader(std::size_t serializerVersion) {
	push();
	LOG_DEBUG << "Serializing script " << currentScriptName() << " with compiled id " << currentScriptId();
	(*this) << serializerVersion;
	(*this) << currentScriptName();
	(*this) << currentScriptId();
	(*this) << static_cast<std::size_t>(currentScriptBridged());
	return m_buffer.size() - 1;
}

std::pair<std::size_t, std::vector<std::string>> ska::bytecode::SerializationContext::writeInstructions() {
	push();
	auto linkedScripts = std::vector<std::string>{};
	(*this) << instructionsSize();
	for (const Instruction& instruction : (*this)) {
		LOG_DEBUG << "Serializing " << instruction;
		(*this) << instruction;
		if (instruction.command() == Command::SCRIPT) {
			linkedScripts.push_back(scriptName(instruction.left().as<ScriptVariableRef>().variable));
		}
	}
	
	return std::make_pair(m_buffer.size() - 1, std::move(linkedScripts));
}

std::size_t ska::bytecode::SerializationContext::writeExternalReferences(std::vector<std::string> linkedScripts) {
	push();
	(*this) << linkedScripts.size();
	for (const auto& linkedScript : linkedScripts) {
		LOG_INFO << linkedScript;
		(*this) << linkedScript;
	}
	return m_buffer.size() - 1;
}

std::size_t ska::bytecode::SerializationContext::writeSymbolTable() {
	push();
	m_symbolsSerializer.writeFull(m_id, m_natives, buffer());
	return m_buffer.size() - 1;
}

std::size_t ska::bytecode::SerializationContext::writeExports() {
	push();
	auto& exports = m_cache[m_id].exportedSymbols();
	LOG_INFO << "Export serializing : " << exports.size();
	(*this) << exports.size();
	for (const auto& exp : exports) {
		if (!exp.value.empty()) {
			m_symbolsSerializer.writeIfExists(buffer(), m_natives, exp.symbol);
			(*this) << exp.value;
		}
	}
	return m_buffer.size() - 1;
}

void ska::bytecode::SerializationContext::operator<<(std::size_t value) {
	CommonSerializer::write(buffer(), value);
}

void ska::bytecode::SerializationContext::operator<<(std::string value) {
	m_natives.emplace(value, m_natives.size());
	auto refIndex = m_natives.at(value);
	LOG_DEBUG << "Writing native index " << refIndex << " for value " << value;
	buffer().write(reinterpret_cast<const char*>(&refIndex), sizeof(Chunk));
}

void ska::bytecode::SerializationContext::operator<<(const Instruction& value) {
	uint16_t cmd = static_cast<uint16_t>(value.command());
	uint8_t numberOfValidOperands = !value.dest().empty() + !value.left().empty() + !value.right().empty();
	buffer().write(reinterpret_cast<const char*>(&cmd), sizeof(uint16_t));
	buffer().write(reinterpret_cast<const char*>(&numberOfValidOperands), sizeof(uint8_t));

	if (numberOfValidOperands-- > 0) *this << value.dest(); else return;
	if (numberOfValidOperands-- > 0) *this << value.left(); else return;
	if (numberOfValidOperands-- > 0) *this << value.right();
}

void ska::bytecode::SerializationContext::operator<<(const Operand& value) {
	OperandSerializer::write(m_cache, buffer(), m_natives, value);
}

void ska::bytecode::SerializationContext::commit(std::deque<std::size_t> partIndexes) {
	auto target = std::vector<std::stringstream>();	
	assert(partIndexes.size() == m_buffer.size());
	for (auto index : partIndexes) {
		if (index < m_buffer.size()) {
			target.push_back(std::move(m_buffer[index]));
			m_buffer[index] = std::stringstream{};
		}
	}

	for (auto& buf : target) {
		(*m_output) << buf.rdbuf();
	}
	m_buffer.clear();
}

void ska::bytecode::SerializationContext::push() {
	m_buffer.emplace_back();
}

std::size_t ska::bytecode::SerializationContext::pushNatives() {
	push();
	if (m_natives.size() != 0) {
		auto nativeVector = std::vector<std::string>(m_natives.size());
		LOG_DEBUG << "Total of " << nativeVector.size() << " natives";
		for (auto& [native, index] : m_natives) {
			LOG_DEBUG << "Building native vector [" << index << "] = " << native;
			nativeVector[index] = native;
		}

		const auto totalSize = nativeVector.size();
		LOG_INFO << "Natives : " << totalSize;
		
		buffer().write(reinterpret_cast<const char*>(&totalSize), sizeof(uint32_t));
		
		for (auto& native : nativeVector) {
			LOG_INFO << native;
			const auto size = native.size();
			buffer().write(reinterpret_cast<const char*>(&size), sizeof(Chunk));
			if(size > 0) {
				buffer().write(native.c_str(), sizeof(char) * size);
			}
		}
	}
	m_natives.clear();
	return m_buffer.size() - 1;
}
