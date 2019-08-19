#include "InterpreterCommandMov.h"

namespace ska {

	namespace bytecode {
	static NodeValue TokenVariantFromValue(ExecutionContext& context, Value& value) {
		auto output = NodeValue{};
		std::visit([&](const auto& content) {
			using TypeT = std::decay_t<decltype(content)>;
			if constexpr (std::is_same_v<std::size_t, TypeT>) {
				output = std::move(context.getVariant(value));
			} else {
				output = content;
			}
		}, value.content);
		return output;
	}
}
}
SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(MOV)(ExecutionContext& context, Value& left, Value& right) {
  return TokenVariantFromValue(context, left);
}
