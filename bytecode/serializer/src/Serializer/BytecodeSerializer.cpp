#include "Config/LoggerConfigLang.h"
#include "BytecodeSerializer.h"

SKA_LOGC_CONFIG(ska::LogLevel::Info, ska::bytecode::Serializer);

#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::Serializer)
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::bytecode::Serializer)

void ska::bytecode::Serializer::serialize(SerializationContext& context) const {
	do {
		LOG_INFO << "[Script " << context.currentScriptName() << "]";
		for (const auto& instruction : context) {
			LOG_DEBUG << "Serializing " << instruction;
			context << static_cast<std::size_t>(instruction.command());
			context << " ";
			serialize(instruction.dest(), context);
			context << " ";
			serialize(instruction.left(), context);
			context << " ";
			serialize(instruction.right(), context);
			context << "\n";
		}
		context << "\n\n";
		const auto& exports = context.exports();
		LOG_INFO << "Export serializing : " << exports.size();
		for (const auto& exp : exports) {
			serialize(exp, context);
			context << " ";
		}
	} while (context.next());
}

void ska::bytecode::Serializer::serialize(const Operand& operand, SerializationContext& output) const {
	if (operand.empty()) {
		return;
	}
	const auto& content = operand.content();
	if (std::holds_alternative<StringShared>(content)) {
		output << *std::get<StringShared>(content);
	}

	output << static_cast<std::size_t>(operand.type());

	std::visit([&output](const auto& operand) {
		using TypeT = std::decay_t<decltype(operand)>;
		if constexpr (std::is_same_v<ScriptVariableRef, TypeT>) {
			if (output.currentScriptId() != operand.script) {
				output << operand.script;
			}
			output << operand.variable;
		} else if constexpr (!std::is_same_v<StringShared, TypeT>) {
			output << operand;
		}
	}, content);
}

void ska::bytecode::Serializer::serialize(const ScriptCache& cache, std::size_t generatedId, std::ostream& output) const {
	auto context = SerializationContext { cache, generatedId, output };
	serialize(context);
}
