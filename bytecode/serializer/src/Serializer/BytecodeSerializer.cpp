#include "Config/LoggerConfigLang.h"
#include "BytecodeSerializer.h"

static constexpr std::size_t SERIALIZER_VERSION = 1;

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::bytecode::Serializer);

#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::Serializer)
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::bytecode::Serializer)

void ska::bytecode::Serializer::serialize(SerializationContext& context) const {
	do {
		LOG_INFO << "[Script " << context.currentScriptName() << "]";
		context << context.currentScriptName();
		context << SERIALIZER_VERSION;

		for (const auto& instruction : context) {
			LOG_DEBUG << "Serializing " << instruction;
			context << instruction;
		}
		context << Instruction{ Command::NOP, std::vector<Operand>{} };

		const auto& exports = context.exports();
		LOG_INFO << "Export serializing : " << exports.size();
		for (const auto& exp : exports) {
			if(!exp.empty()) {
				context << exp;
			}
		}
		context << Operand{};

	} while (context.next());

}

void ska::bytecode::Serializer::deserialize(DeserializationContext& context) const {
	while(context.canRead()) {
		auto nativesRef = std::vector<std::size_t>{};
		auto scriptRef = Chunk {};
		auto serializerVersion = std::size_t {};
		context >> scriptRef;
		context >> serializerVersion;
		nativesRef.push_back(std::move(scriptRef));

		LOG_INFO << "[Serializer script version " << serializerVersion << "]";

		auto instructions = std::vector<Instruction>{};
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

		auto exports = std::vector<Operand>{};
		auto operandType = OperandType {};
		do {
			auto operand = Operand{};
			context >> operand;
			operandType = operand.type();
			if (!operand.empty()) {
				LOG_INFO << "Getting export " << operand;
				exports.push_back(std::move(operand));
			}
		} while (operandType != OperandType::EMPTY);

		LOG_INFO << "Natives section ";

		auto natives = std::vector<std::string>{};
		context >> natives;

		replaceAllNativesRef(instructions, natives);
		replaceAllNativesRef(exports, natives);

		if(!natives.empty()) {
			LOG_INFO << "[Script name " << natives[static_cast<std::size_t>(scriptRef)] << "]";
			context.declare(natives[static_cast<std::size_t>(scriptRef)], std::move(instructions));
			context.exports(std::move(exports));
		}
	}

}

void ska::bytecode::Serializer::replaceAllNativesRef(Operand& operand, const std::vector<std::string>& natives) const {
	if (operand.type() == OperandType::MAGIC) {
		auto realValue = natives[operand.as<ScriptVariableRef>().variable];
		operand = Operand{ std::make_shared<std::string>(std::move(realValue)), OperandType::PURE };
	}
}

void ska::bytecode::Serializer::replaceAllNativesRef(std::vector<Operand>& operands, const std::vector<std::string>& natives) const {
	for (auto& operand : operands) {
		if (operand.type() == OperandType::MAGIC) {
			auto realValue = natives[operand.as<ScriptVariableRef>().variable];
			operand = Operand { std::make_shared<std::string>(std::move(realValue)), OperandType::PURE };
		}
	}
}

void ska::bytecode::Serializer::replaceAllNativesRef(std::vector<Instruction>& instructions, const std::vector<std::string>& natives) const {
	for (auto& instruction : instructions) {
		replaceAllNativesRef(instruction.dest(), natives);
		replaceAllNativesRef(instruction.left(), natives);
		replaceAllNativesRef(instruction.right(), natives);
	}
}

void ska::bytecode::Serializer::serialize(const ScriptCache& cache, std::ostream& output) const {
	auto context = SerializationContext { cache, 0, output };
	serialize(context);
}

void ska::bytecode::Serializer::deserialize(ScriptCache& cache, std::istream& input) const {
	auto context = DeserializationContext{ cache, input };
	deserialize(context);
}
