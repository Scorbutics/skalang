#include "Config/LoggerConfigLang.h"
#include "BytecodeDeserializationContext.h"
#include "BytecodeScriptExternalReferences.h"
#include "BytecodeScriptHeader.h"
#include "BytecodeScriptBody.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::bytecode::DeserializationContext);

#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::DeserializationContext)
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::bytecode::DeserializationContext)

void ska::bytecode::DeserializationContext::declare(std::string scriptName, std::vector<Instruction> instructions, std::vector<Operand> exports) {
	auto output = InstructionOutput {};
	for(auto& ins : instructions) {
		output.push(std::move(ins));
	}
	auto fakeAST = ska::ScriptAST{ m_cache.astCache, scriptName, {Token {}} };
	m_cache.emplace(scriptName, ScriptGeneration{ ScriptGenerationHelper{ m_cache, fakeAST}, std::move(output) });
	m_cache.at(scriptName).setExportedSymbols(std::move(exports));
}

void ska::bytecode::DeserializationContext::operator>>(ScriptHeader& header) {
	(*this) >> header.serializerVersion;
	(*this) >> header.scriptNameRef;
	(*this) >> header.scriptId;
	(*this) >> header.scriptBridged;
}

void ska::bytecode::DeserializationContext::operator>>(ScriptBody& body) {
	body.instructions = readInstructions();
	replaceAllNativesRef(body.instructions, body.natives());
	body.exports = readExports();
	replaceAllNativesRef(body.exports, body.natives());
}

void ska::bytecode::DeserializationContext::operator>>(ScriptExternalReferences& externalReferences) {
	externalReferences.scripts = readLinkedScripts(externalReferences.natives());
}

void ska::bytecode::DeserializationContext::replaceAllNativesRef(Operand& operand, const std::vector<std::string>& natives) const {
	switch (operand.type()) {
	case OperandType::MAGIC: {
		auto realValue = natives[operand.as<ScriptVariableRef>().variable];
		operand = Operand{ std::make_shared<std::string>(std::move(realValue)), OperandType::PURE };
	} break;

	case OperandType::BIND_NATIVE:
	case OperandType::REG:
	case OperandType::VAR: {
		auto scriptName = natives[operand.as<ScriptVariableRef>().script];
		auto realValue = operand.as<ScriptVariableRef>();
		realValue.script = m_cache.id(scriptName);
		operand = Operand{ std::move(realValue), operand.type() };
	} break;
	
	case OperandType::BIND_SCRIPT: {
		auto scriptName = natives[operand.as<ScriptVariableRef>().script];
		auto scriptId = m_cache.id(scriptName);
		auto scriptReferedName = natives[operand.as<ScriptVariableRef>().variable];
		auto scriptReferedId = m_cache.id(scriptReferedName);
		auto realValue = ScriptVariableRef{ scriptReferedId, scriptId };
		operand = Operand{ std::move(realValue), operand.type() };
	} break;

	default:
		break;
	}
}

void ska::bytecode::DeserializationContext::replaceAllNativesRef(std::vector<Operand>& operands, const std::vector<std::string>& natives) const {
	for (auto& operand : operands) {
		replaceAllNativesRef(operand, natives);
	}
}

void ska::bytecode::DeserializationContext::replaceAllNativesRef(std::vector<Instruction>& instructions, const std::vector<std::string>& natives) const {
	for (auto& instruction : instructions) {
		replaceAllNativesRef(instruction.dest(), natives);
		replaceAllNativesRef(instruction.left(), natives);
		replaceAllNativesRef(instruction.right(), natives);
	}
}

std::vector<ska::bytecode::Instruction> ska::bytecode::DeserializationContext::readInstructions() {
	auto instructionsSize = std::size_t{};
	(*this) >> instructionsSize;

	if (instructionsSize == 0) {
		return {};
	}

	auto instructions = std::vector<Instruction>(instructionsSize);
	for (std::size_t i = 0; i < instructionsSize; i++){
		(*this) >> instructions[i];
		LOG_INFO << "Deserializing " << instructions[i];
	}
	return instructions;
}

std::unordered_set<std::string> ska::bytecode::DeserializationContext::readLinkedScripts(const std::vector<std::string>& natives) {
	auto linkedScriptsRef = std::unordered_set<std::string>{};
	auto linkedScriptsRefSize = std::size_t{};
	(*this) >> linkedScriptsRefSize;

	LOG_INFO << "Linked scripts section : " << linkedScriptsRefSize;

	auto scriptRef = Chunk{};
	for (std::size_t i = 0; i < linkedScriptsRefSize; i++) {
		(*this) >> scriptRef;
		LOG_INFO << "Getting script reference " << natives[scriptRef];
		linkedScriptsRef.insert(natives[scriptRef]);
	}
	return linkedScriptsRef;
}

std::vector<ska::bytecode::Operand> ska::bytecode::DeserializationContext::readExports() {
	auto exportsSize = std::size_t{};
	(*this) >> exportsSize;
	LOG_INFO << "Exports section : " << exportsSize;
	if (exportsSize == 0) {
		return {};
	}
	auto exports = std::vector<Operand>(exportsSize);
	for(std::size_t i = 0; i < exportsSize; i++) {
		(*this) >> exports[i];		
		LOG_INFO << "Getting export " << exports[i];
	};
	return exports;
}

void ska::bytecode::DeserializationContext::operator>>(std::size_t& value) {
	value = 0;
	checkValidity();
	if(m_input->eof()) {
		return;
	}
	m_input->read(reinterpret_cast<char*>(&value), sizeof(uint32_t));
}

void ska::bytecode::DeserializationContext::operator>>(Chunk& value) {
	checkValidity();
	if(m_input->eof()) {
		value = Chunk{};
		return;
	}
	m_input->read(reinterpret_cast<char*>(&value), sizeof(Chunk));
}

void ska::bytecode::DeserializationContext::operator>>(Instruction& value) {
	checkValidity();
	auto operands = std::vector<Operand>{};
	auto numberOfValidOperands = std::uint8_t{ 0 };
	auto command = uint16_t { 0 };

	if(m_input->eof()) {
		value = Instruction{};
		return;
	}

	m_input->read(reinterpret_cast<char*>(&command), sizeof(uint16_t));
	m_input->read(reinterpret_cast<char*>(&numberOfValidOperands), sizeof(uint8_t));
	for(uint8_t i = 0; i < numberOfValidOperands; i++) {
		auto tmp = Operand {};
		*this >> tmp;
		operands.push_back(std::move(tmp));
	}
	value = Instruction{ static_cast<Command>(command), std::move(operands) };
}

void ska::bytecode::DeserializationContext::operator>>(Operand& value) {
	checkValidity();
	auto type = uint8_t { 0 };
	auto script = Chunk{ 0 };
	auto variable = Chunk{ 0 };

	if(m_input->eof()) {
		value = Operand{};
		return;
	}

	m_input->read(reinterpret_cast<char*>(&type), sizeof(uint8_t));
	m_input->read(reinterpret_cast<char*>(&script), sizeof(Chunk));
	m_input->read(reinterpret_cast<char*>(&variable), sizeof(Chunk));

	auto operandType = static_cast<OperandType>(type);
	auto content = OperandVariant {};
	switch (operandType) {
		case OperandType::PURE:
			switch (static_cast<std::size_t>(script)) {
			case 0:
				//native ref
				content = ScriptVariableRef { static_cast<std::size_t>(variable), static_cast<std::size_t>(0) };
				operandType = OperandType::MAGIC;
				break;
			case 1:
				content = static_cast<long>(variable);
				break;
			case 2:
				content = static_cast<double>(variable);
				break;
			case 3:
				content = static_cast<bool>(variable);
				break;
			}
		break;

		case OperandType::BIND_SCRIPT:
		case OperandType::BIND_NATIVE:
		case OperandType::REG:
		case OperandType::VAR:
			content = ScriptVariableRef{ static_cast<std::size_t>(variable), static_cast<std::size_t>(script) };
			break;

		case OperandType::EMPTY:
			value = Operand {};
			return;

		default:
			throw std::runtime_error("cannot handle operand type deserialization " + std::to_string(type));
			break;
	}
	value = Operand{ std::move(content), operandType };
}
void ska::bytecode::DeserializationContext::operator>>(std::vector<std::string>& natives) {
	checkValidity();
	if(m_input->eof()){
		return;
	}

	auto nativesSize = std::size_t{ };
	(*this) >> nativesSize;
	for (std::size_t i = 0; i < nativesSize; i++) {
		auto native = readString();
		LOG_INFO << i << "\t: " << native.value();
		natives.push_back(std::move(native.value()));		
	}
}

std::optional<std::string> ska::bytecode::DeserializationContext::readString() {
	auto value = std::string {};
	auto size = Chunk { 0 };
	m_input->read(reinterpret_cast<char*>(&size), sizeof(Chunk));
	if (size > 0) {
		value.resize(size);
		m_input->read(reinterpret_cast<char*>(&value[0]), sizeof(char)* size);
		return value;
	}
	return {};
}