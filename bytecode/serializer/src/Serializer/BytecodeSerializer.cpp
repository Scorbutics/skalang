#include "Config/LoggerConfigLang.h"
#include "BytecodeSerializer.h"

static constexpr auto SERIALIZER_VERSION = 1;

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::bytecode::Serializer);

#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::Serializer)
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::bytecode::Serializer)

void ska::bytecode::Serializer::serialize(SerializationContext& context) const {
	do {
		LOG_INFO << "[Script " << context.currentScriptName() << "]";
		auto natives = std::vector<std::string>{};
		context << context.currentScriptName();
		context << SERIALIZER_VERSION;

		for (const auto& instruction : context) {
			LOG_DEBUG << "Serializing " << instruction;
			serialize(instruction.command(), instruction, context, natives);
		}
		serialize(Command::NOP, {}, context, natives);

		const auto& exports = context.exports();
		LOG_INFO << "Export serializing : " << exports.size();
		for (const auto& exp : exports) {
			serialize(exp, context, natives);
		}
		serialize({}, context, natives);

		LOG_INFO << "Natives serializing : " << natives.size();
		for (auto& native : natives) {
			if (!native.empty()) {
				context << std::move(native);
			}
		}
		context << "";
	} while (context.next());

}

void ska::bytecode::Serializer::serialize(Command command, const Instruction& instruction, SerializationContext& output, std::vector<std::string>& natives) const {
	output << static_cast<std::size_t>(command);
	serialize(instruction.dest(), output, natives);
	serialize(instruction.left(), output, natives);
	serialize(instruction.right(), output, natives);
}

void ska::bytecode::Serializer::serialize(const Operand& operand, SerializationContext& output, std::vector<std::string>& natives) const {
	Chunk type = static_cast<Chunk>(OperandType::EMPTY);
	Chunk script {};
	Chunk variable {};

	const auto& content = operand.content();
	if (std::holds_alternative<StringShared>(content)) {
		type = static_cast<Chunk>(OperandType::PURE);
		script = 0;
		variable = natives.size();
		natives.push_back(*std::get<StringShared>(content));
	} else if (!operand.empty()) {
		type = static_cast<Chunk>(operand.type());

		std::visit([&](const auto& operand) {
			using TypeT = std::decay_t<decltype(operand)>;
			if constexpr (std::is_same_v<ScriptVariableRef, TypeT>) {
				script = operand.script;
				variable = operand.variable;
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
	}

	output << type;
	output << script;
	output << variable;
}

void ska::bytecode::Serializer::deserialize(DeserializationContext& context, std::vector<std::string>& natives) const {
	while(context.canRead()) {
		auto nativesRef = std::vector<Chunk>{};
		auto scriptRef = std::size_t {};
		auto serializerVersion = std::size_t {};
		context >> scriptRef;
		context >> serializerVersion;
		nativesRef.push_back(std::move(scriptRef));

		auto instructions = std::vector<Instruction>{};
		auto command = Command{};
		do {
			auto instruction = Instruction{};
			context >> instruction;
			command = instruction.command();
			if (command != Command::NOP) {
				instructions.push_back(std::move(instruction));
			}
		} while (command != Command::NOP);

		auto exports = std::vector<Operand>{};
		auto operandType = OperandType {};
		do {
			auto operand = Operand{};
			context >> operand;
			operandType = operand.type();
			exports.push_back(std::move(operand));
		} while (operandType != OperandType::EMPTY);

		auto natives = std::vector<std::string>{};
		do {
			auto native = std::string{};
			context >> native;
			natives.push_back(std::move(native));
		} while (!natives.back().empty());

		replaceAllNativesRef(instructions, natives);
		replaceAllNativesRef(exports, natives);
	}

}

void ska::bytecode::Serializer::replaceAllNativesRef(Operand& operand, const std::vector<std::string>& natives) const {
	if (operand.type() == OperandType::BIND) {
		auto realValue = natives[operand.as<ScriptVariableRef>().variable];
		operand = Operand{ std::make_shared<std::string>(std::move(realValue)), OperandType::PURE };
	}	
}

void ska::bytecode::Serializer::replaceAllNativesRef(std::vector<Operand>& operands, const std::vector<std::string>& natives) const {
	for (auto& operand : operands) {
		if (operand.type() == OperandType::BIND) {
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
	auto context = DeserializationContext{ cache, 0, input };
	std::vector<std::string> natives;
	deserialize(context, natives);
}
