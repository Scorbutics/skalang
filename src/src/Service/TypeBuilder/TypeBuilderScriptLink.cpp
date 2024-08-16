#include "Config/LoggerConfigLang.h"
#include "TypeBuilderScriptLink.h"
#include "NodeValue/AST.h"
#include "Service/SymbolTable.h"
#include "NodeValue/ScriptAST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::SCRIPT_LINK>)

ska::TypeHierarchy ska::TypeBuilderOperator<ska::Operator::SCRIPT_LINK>::build(const ScriptAST& script, OperateOn node) {
	auto& symbols = script.symbols();
	auto* symbolTable = symbols[node.GetValue().name()];
    SLOG(LogLevel::Info) << "Looking for script cache \"" << node.GetValue().name() << "\" with symbol " << (symbolTable == nullptr ? "" : symbolTable->name());
	if(symbolTable == nullptr) {
		return Type::MakeBuiltIn(ExpressionType::VOID);
	}
	return { Type::MakeCustom<ExpressionType::OBJECT>(nullptr), *symbolTable };
}
