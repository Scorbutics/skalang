#include "Config/LoggerConfigLang.h"
#include "TypeBuilderScriptObject.h"
#include "NodeValue/AST.h"
#include "Service/SymbolTable.h"
#include "NodeValue/ScriptAST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::SCRIPT_OBJECT>)
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::TypeBuilderOperator<ska::Operator::SCRIPT_OBJECT>)
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::SCRIPT_OBJECT>)

ska::TypeHierarchy ska::TypeBuilderOperator<ska::Operator::SCRIPT_OBJECT>::build(const ScriptAST& script, OperateOn node) {
	auto& symbols = script.symbols();
	auto scriptType = Type::MakeCustom<ExpressionType::FUNCTION>(nullptr);

	for (auto& fieldNode : node) {
		LOG_INFO << fieldNode->name();
		if (fieldNode->symbol() != nullptr) {
			scriptType.add(fieldNode->symbol()->type());
		}
	}

	LOG_DEBUG << "Script object declaration \""<< script.name() <<"\" with type "<< scriptType;
	return { scriptType, script.symbols().root() } ;
}