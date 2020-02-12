#include <unordered_set>
#include "Config/LoggerConfigLang.h"
#include "BytecodeSerializer.h"

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
		auto scriptNameRef = Chunk{};
		auto serializerVersion = std::size_t{};

		auto scriptBridged = std::size_t{};
		auto scriptId = std::size_t{ 0 };
		context >> serializerVersion;
		context >> scriptNameRef;
		context >> scriptId;
		context >> scriptBridged;

		LOG_INFO << "[Serializer script version " << serializerVersion << "]";

		auto instructions = std::vector<Instruction>{};
		auto exports = std::vector<Operand>{};
		auto linkedScriptsRef = std::vector<Chunk>{};

		if (!scriptBridged) {

			auto command = Command{};
			do {
				auto instruction = Instruction{};
				context >> instruction;
				command = instruction.command();
				if (command != Command::NOP) {
					LOG_INFO << "Deserializing " << instruction;
					instructions.push_back(std::move(instruction));
				}
			} while (command != Command::NOP);

			LOG_INFO << "Exports section ";

			auto operandType = OperandType{};
			do {
				auto operand = Operand{};
				context >> operand;
				operandType = operand.type();
				if (!operand.empty()) {
					LOG_INFO << "Getting export " << operand;
					exports.push_back(std::move(operand));
				}
			} while (operandType != OperandType::EMPTY);

			auto linkedScriptsRefSize = std::size_t{};
			context >> linkedScriptsRefSize;
			LOG_INFO << "Linked scripts section : " << linkedScriptsRefSize;
			auto scriptRef = Chunk{};
			for(std::size_t i = 0; i < linkedScriptsRefSize; i++) {
				context >> scriptRef;
				LOG_INFO << "Getting script " << scriptRef;
				linkedScriptsRef.push_back(scriptRef);
			}

		}
		LOG_INFO << "Natives section ";

		auto natives = std::vector<std::string>{};
		context >> natives;

		replaceAllNativesRef(instructions, natives);
		replaceAllNativesRef(exports, natives);
		for (const auto& linkedScriptRef : linkedScriptsRef) {
			scripts.emplace(natives[linkedScriptRef]);
		}

		if (!natives.empty()) {
			scriptName = natives[static_cast<std::size_t>(scriptNameRef)];
			scripts.erase(scriptName);
			LOG_INFO << "[Script name " << scriptName << " with id " << scriptId << "]";
			context.declare(scriptId, scriptName, std::move(instructions), std::move(exports));
			auto nextScriptIt = scripts.begin();
			if (nextScriptIt == scripts.end()) {
				break;
			}
			scriptName = *nextScriptIt;
		} else {
			scriptName = "";
		}
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
