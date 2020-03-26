#include "Config/LoggerConfigLang.h"
#include "BytecodeSerializationContext.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::SerializationContext);

#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::SerializationContext)
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::bytecode::SerializationContext)

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

std::size_t ska::bytecode::SerializationContext::writeExports() {
	push();
	auto& exports = m_cache[m_id].exportedSymbols();
	LOG_INFO << "Export serializing : " << exports.size();
	(*this) << exports.size();
	for (const auto& exp : exports) {
		if (!exp.value.empty()) {
			//(*this) << exp.symbol;
			(*this) << exp.value;
		}
	}
	return m_buffer.size() - 1;
}

void ska::bytecode::SerializationContext::operator<<(std::size_t value) {
	buffer().write(reinterpret_cast<const char*>(&value), sizeof(uint32_t));
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

void ska::bytecode::SerializationContext::operator<<(const Symbol* value) {
	if (value == nullptr) {
		throw std::runtime_error("cannot serialize a null symbol");
	}

	auto operand = extractGeneratedOperandFromSymbol(*value);
	LOG_INFO << "%13cName : " << value->name();
	LOG_INFO << "%13c\twith Raw operand " << operand;
	LOG_INFO << "%13c\twith " << value->type().size() << " children";
	LOG_INFO << "%13c\twith Raw type : " << ExpressionTypeSTR[static_cast<std::size_t>(value->type().type())];
	*this << operand;
	if (m_symbols.find(value) == m_symbols.end()) {
		m_symbols.insert(value);
		*this << value->type();
	} else {
		LOG_DEBUG << "Symbol \"" << value->name() << "\" already registered, linked";
	}
}

ska::bytecode::Operand ska::bytecode::SerializationContext::extractGeneratedOperandFromSymbol(const Symbol& symbol) {
	auto* info = m_cache.getSymbolInfo(symbol);
	if (info == nullptr) {
		throw std::runtime_error("unknown ast symbol \"" + symbol.name() + "\" detected during script bytecode serialization");
	}
	auto operand = m_cache[info->script].getSymbol(symbol);
	if (!operand.has_value()) {
		throw std::runtime_error("not generated symbol \"" + symbol.name() + "\" in ast detected during script bytecode serialization");
	}

	LOG_DEBUG << "Symbol \"" << symbol.name() << "\" as variable \"" << operand.value() << "\"";

	return operand.value();
}

void ska::bytecode::SerializationContext::operator<<(const Type value) {
	auto rawType = value.type();
	buffer().write(reinterpret_cast<const char*>(&rawType), sizeof(uint8_t));

	LOG_INFO << "Type \"" << value << "\" is being serialized with " << value.size() << " compound types";

	auto index = std::size_t{0};
	for (auto& childType : value) {
		auto operand = Operand{};
		LOG_INFO << "\t\tChild type " << childType;
		/*if (childType.hasSymbol()) {
			operand = extractGeneratedOperandFromSymbol(*childType.symbol());
		}
		LOG_INFO << "%13cChild " << index << "\t\twith name "<< (childType.symbol() ? childType.symbol()->name() : "");*/
		LOG_INFO << "%13c\t\twith Raw operand " << operand;
		LOG_INFO << "%13c\t\twith " << childType.size() << " children";
		LOG_INFO << "%13c\t\twith Raw type : " << ExpressionTypeSTR[static_cast<std::size_t>(childType.type())];
		*this << static_cast<std::size_t>(childType.type());
		*this << childType.size();
		*this << operand;
		*this << childType;
		index++;
	}
}

void ska::bytecode::SerializationContext::operator<<(const Operand& value) {
	uint8_t type = static_cast<uint8_t>(OperandType::EMPTY);
	Chunk script { 0 };
	Chunk variable { 0 };

	if (value.empty()) {
		buffer().write(reinterpret_cast<const char*>(&type), sizeof(uint8_t));
		char empty[sizeof(Chunk) * 2] = "";
		buffer().write(empty, sizeof(empty));
		return;
	}

	const auto& content = value.content();
	if (std::holds_alternative<StringShared>(content)) {
		type = static_cast<uint8_t>(OperandType::PURE);
		script = 0;
		buffer().write(reinterpret_cast<const char*>(&type), sizeof(uint8_t));
		buffer().write(reinterpret_cast<const char*>(&script), sizeof(Chunk));
		*this << *std::get<StringShared>(content);
	} else {
		type = static_cast<uint8_t>(value.type());

		std::visit([&](const auto& operand) {
			using TypeT = std::decay_t<decltype(operand)>;
			if constexpr (std::is_same_v<ScriptVariableRef, TypeT>) {
				m_natives.emplace(scriptName(operand.script), m_natives.size());
				script = m_natives.at(scriptName(operand.script));
				
				if (value.type() == OperandType::BIND_SCRIPT) {
					m_natives.emplace(scriptName(operand.variable), m_natives.size());
					variable = m_natives.at(scriptName(operand.variable));
				} else {
					variable = operand.variable;
				}
			} else if constexpr (!std::is_same_v<StringShared, TypeT>) {
				if constexpr (std::is_same_v<long, TypeT>) {
					script = static_cast<Chunk>(1);
				} else if constexpr (std::is_same_v<double, TypeT>) {
					script = static_cast<Chunk>(2);
				} else {
					script = static_cast<Chunk>(3);
				}
				variable = static_cast<Chunk>(operand);
			}
		}, content);

		buffer().write(reinterpret_cast<const char*>(&type), sizeof(uint8_t));
		buffer().write(reinterpret_cast<const char*>(&script), sizeof(Chunk));
		buffer().write(reinterpret_cast<const char*>(&variable), sizeof(Chunk));
	}
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
	m_symbols.clear();
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
			nativeVector[index] = std::move(native);
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
