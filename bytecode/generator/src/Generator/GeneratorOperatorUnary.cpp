#include "Config/LoggerConfigLang.h"
#include <string>
#include "NodeValue/AST.h"
#include "GeneratorOperatorUnary.h"
#include "NodeValue/ScriptAST.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScript.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::bytecode::GeneratorOperator<ska::Operator::UNARY>);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::GeneratorOperator<ska::Operator::UNARY>)

namespace ska {
	namespace bytecode {
		static GenerationOutput CommonGenerate(GenerationContext& context) {
			const auto& node = context.pointer();
			auto info = SymbolInfo {};
			if(node.symbol() != nullptr) {
				auto tmpInfo = context.script().getSymbolInfo(*node.symbol());
				if(tmpInfo != nullptr) {
					info = *tmpInfo;
				}
				LOG_DEBUG << "Symbol info for \"" << node.symbol()->getName() << "\" node : " << info;
			} else {
				LOG_DEBUG << "No symbol info";
			}

			auto result = context.script().querySymbolOrValue(node);
			return { std::move(result), std::move(info) };
		}
	}
}

ska::bytecode::GenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::UNARY>::generate(OperateOn node, GenerationContext& context) {
	return CommonGenerate(context);
}

ska::bytecode::GenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::LITERAL>::generate(OperateOn node, GenerationContext& context) {
	return CommonGenerate(context);
}
