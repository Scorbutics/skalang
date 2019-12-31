#include "Config/LoggerConfigLang.h"
#include "InterpreterCommandJumpBind.h"

using InterpreterCommand = ska::bytecode::InterpreterCommand<ska::bytecode::Command::BIND>;

SKA_LOGC_CONFIG(ska::LogLevel::Debug, InterpreterCommand);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, InterpreterCommand)

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(BIND)(ExecutionContext& context, const Operand& left, const Operand& right) {
	const auto bindingIndex = context.get<long>(context.currentInstruction().dest());
	const auto parametersNumber = context.get<long>(left);

	LOG_DEBUG << "Jumping (binding) number " << bindingIndex << " with " << parametersNumber
		<< " parameters to native function in script " << context.currentScriptId();
	
	const auto& nativeFunction = context.getBinding(bindingIndex);
	auto parameterValues = std::make_shared<NodeValueArrayRaw>();
	context.pop(*parameterValues, parametersNumber);
	auto parametersAsVector = std::vector<ska::NodeValue>{};
	std::copy(parameterValues->begin(), parameterValues->end(), std::back_inserter(parametersAsVector));
	auto result = nativeFunction.function(std::move(parametersAsVector));
	if (!result.empty()) {
		context.push(std::move(result));
	}
	return {};
}
