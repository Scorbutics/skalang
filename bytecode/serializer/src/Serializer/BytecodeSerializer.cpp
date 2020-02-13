#include "Config/LoggerConfigLang.h"
#include <unordered_set>
#include "BytecodeSerializer.h"
#include "BytecodeSerializationContext.h"
#include "BytecodeDeserializationContext.h"

#include "BytecodeScriptHeader.h"
#include "BytecodeScriptBody.h"
#include "BytecodeScriptExternalReferences.h"

static constexpr std::size_t SERIALIZER_VERSION = 1;

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::bytecode::Serializer);

#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::Serializer)
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::bytecode::Serializer)

bool ska::bytecode::Serializer::serialize(SerializationContext& context) const {
	try {
		do {
			LOG_INFO << "[Script " << context.currentScriptName() << "]";
			context << SERIALIZER_VERSION;
			context << context.currentScriptName();
			context << context.currentScriptId();
			context << static_cast<std::size_t>(context.currentScriptBridged());

			if (!context.currentScriptBridged()) {
				auto linkedScripts = std::vector<std::string>{};
				for (const auto& instruction : context) {
					LOG_DEBUG << "Serializing " << instruction;
					if (instruction.command() == Command::SCRIPT) {
						linkedScripts.push_back(context.scriptName(instruction.left().as<ScriptVariableRef>().variable));
					}
					context << instruction;
				}
				context << Instruction{ Command::NOP, std::vector<Operand>{} };

				const auto& exports = context.exports();
				LOG_INFO << "Export serializing : " << exports.size();
				for (const auto& exp : exports) {
					if (!exp.empty()) {
						LOG_INFO << exp;
						context << exp;
					}
				}
				context << Operand{};

				LOG_INFO << "Linked scripts serializing : " << exports.size();
				context << linkedScripts.size();
				for (const auto& linkedScript : linkedScripts) {
					LOG_INFO << linkedScript;
					context << linkedScript;
				}
			}
		} while (context.next());
		return true;
	} catch (std::runtime_error&) {
		return false;
	}
}

bool ska::bytecode::Serializer::deserialize(DeserializationContext& context) const {
	std::string scriptName = context.startScriptName();
	auto scripts = std::unordered_set<std::string> {};
	auto wasRead = false;
	while (context.read(scriptName)) {
		wasRead = true;
		
		auto header = ScriptHeader{};
		context >> header;

		LOG_INFO << "[Serializer script version " << header.serializerVersion << "], script id " << header.scriptId;

		auto body = ScriptBody{};
		if (!header.scriptBridged) {
			context >> body;
		}

		auto externalReferences = ScriptExternalReferences { header, body };
		context >> externalReferences;

		std::copy(externalReferences.scripts.begin(), externalReferences.scripts.end(), std::inserter(scripts, scripts.end()));
		scriptName = header.scriptName;

		scripts.erase(scriptName);

		LOG_INFO << "[Script name " << scriptName << " with id " << header.scriptId << "]";
		context.declare(header.scriptId, scriptName, std::move(body.instructions), std::move(body.exports));

		auto nextScriptIt = scripts.begin();
		if (nextScriptIt == scripts.end()) {
			LOG_INFO << "No more scripts, leaving deserialization";
			break;
		}
		scriptName = *nextScriptIt;
	}
	return wasRead;
}

void ska::bytecode::Serializer::replaceAllNativesRef(Operand& operand, const std::vector<std::string>& natives) const {
	if (operand.type() == OperandType::MAGIC) {
		auto realValue = natives[operand.as<ScriptVariableRef>().variable];
		operand = Operand{ std::make_shared<std::string>(std::move(realValue)), OperandType::PURE };
	}
}

void ska::bytecode::Serializer::replaceAllNativesRef(std::vector<Operand>& operands, const std::vector<std::string>& natives) const {
	for (auto& operand : operands) {
		replaceAllNativesRef(operand, natives);
	}
}

void ska::bytecode::Serializer::replaceAllNativesRef(std::vector<Instruction>& instructions, const std::vector<std::string>& natives) const {
	for (auto& instruction : instructions) {
		replaceAllNativesRef(instruction.dest(), natives);
		replaceAllNativesRef(instruction.left(), natives);
		replaceAllNativesRef(instruction.right(), natives);
	}
}

bool ska::bytecode::Serializer::serialize(const ScriptCache& cache, SerializationStrategy output) const {
	auto context = SerializationContext { cache, output };
	return serialize(context);
}

bool ska::bytecode::Serializer::deserialize(ScriptCache& cache, const std::string& startScriptName, DeserializationStrategy input) const {
	auto context = DeserializationContext{ cache, startScriptName, input };
	return deserialize(context);
}
